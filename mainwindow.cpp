#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
#include "audioplayer.h"
#include "plistreader.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>
#include <QXmlStreamReader>
#include <QtDebug>

static const int TowerCost = 200;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
    , m_waves(0)
	, m_playerHp(5)
	, m_playrGold(1000)
	, m_gameEnded(false)
	, m_gameWin(false)
{
	ui->setupUi(this);

	preLoadWavesInfo();
	loadTowerPositions();

	m_audioPlayer = new AudioPlayer(this);
	m_audioPlayer->startBGM();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);

    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(new_custom()));
    connect(ui->toolButton_2,SIGNAL(clicked()),this,SLOT(new_custom1()));
    connect(ui->toolButton_3,SIGNAL(clicked()),this,SLOT(new_custom2()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::loadTowerPositions()
{
    QFile file(":/config/TowersPosition.plist");
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition.plist");
            return;
        }

        PListReader reader;
        reader.read(&file);

        QList<QVariant> array = reader.data();
        foreach (QVariant dict, array)
        {
            QMap<QString, QVariant> point = dict.toMap();
            int x = point.value("x").toInt();
            int y = point.value("y").toInt();
            m_towerPositionsList.push_back(QPoint(x, y));
        }
        file.close();

        QFile file1(":/config/TowersPosition1.plist");
        if (!file1.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition1.plist");
            return;
        }

        PListReader reader1;
        reader1.read(&file1);

        QList<QVariant> array1 = reader1.data();
        foreach (QVariant dict1, array1)
        {
            QMap<QString, QVariant> point = dict1.toMap();
            int x = point.value("x").toInt();
            int y = point.value("y").toInt();
            m_towerPositionsList1.push_back(QPoint(x, y));
        }
        file1.close();

        QFile file2(":/config/TowersPosition2.plist");
        if (!file2.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition2.plist");
            return;
        }

        PListReader reader2;
        reader2.read(&file2);

        QList<QVariant> array2 = reader2.data();
        foreach (QVariant dict2, array2)
        {
            QMap<QString, QVariant> point = dict2.toMap();
            int x = point.value("x").toInt();
            int y = point.value("y").toInt();
            m_towerPositionsList2.push_back(QPoint(x, y));
        }
        file2.close();
}

void MainWindow::paintEvent(QPaintEvent *)
{
	if (m_gameEnded || m_gameWin)
	{
        QString end_path = m_gameEnded ? ":/image/failure.jpg" : ":/image/victory.jpg";
        QPainter painter(this);
        painter.drawPixmap(0,0,480,320,QPixmap(end_path));
		return;
	}

    //background
    char path[20] = "";
    if(m_waves == 0){
        strcpy(path,":/image/bg1.jpg");
    }
    else if(m_waves == 1){
        strcpy(path,":/image/bg2.jpg");
    }
    else {
        strcpy(path,":/image/bg3.jpg");
    }
    QPixmap cachePix(path);
	QPainter cachePainter(&cachePix);

    //towerposition
    if(m_waves == 0){
    foreach(const TowerPosition &towerPosition,m_towerPositionsList)
    towerPosition.draw(&cachePainter);
    }
    else if (m_waves == 1) {
    foreach(const TowerPosition &towerPosition,m_towerPositionsList1)
    towerPosition.draw(&cachePainter);
    }
    else {
    foreach(const TowerPosition &towerPosition,m_towerPositionsList2)
    towerPosition.draw(&cachePainter);
    }

    foreach (const PurpleTower *tower, m_towersList)
            tower->draw(&cachePainter);

    if(m_waves == 0){
        foreach (const WayPoint *wayPoint, m_wayPointsList)
		wayPoint->draw(&cachePainter);
    }
    else if(m_waves == 1){
        foreach (const WayPoint *wayPoint, m_wayPointsList1)
            wayPoint->draw(&cachePainter);
    }
    else {
        foreach (const WayPoint *wayPoint, m_wayPointsList2)
            wayPoint->draw(&cachePainter);
    }

	foreach (const Enemy *enemy, m_enemyList)
		enemy->draw(&cachePainter);

	foreach (const Bullet *bullet, m_bulletList)
		bullet->draw(&cachePainter);

	drawWave(&cachePainter);
	drawHP(&cachePainter);
	drawPlayerGold(&cachePainter);

	QPainter painter(this);
	painter.drawPixmap(0, 0, cachePix);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(m_waves == 0){
    QPoint pressPos = event->pos();
    auto it = m_towerPositionsList.begin();//按照塔的安放坐标遍历
    while (it != m_towerPositionsList.end())
    {
        if (canBuyTower() && it->containPoint(pressPos))
        {
            m_audioPlayer->playSound(TowerPlaceSound);
            m_playrGold -= TowerCost;
            if (!it->hasBlueTower())
            {
                it->setHasBlueTower();
                BlueTower *tower = new BlueTower(it->centerPos(), this);
                m_towersList.push_back(tower);
            }
            else if (!it->hasPurpleTower() && it->hasBlueTower())
            {
                 it->setHasPurpleTower();
                 for(int i=0;i<m_towersList.size();i++)
                 {
                    if(pressPos.x()<=m_towersList[i]->m_pos.x()+21 && pressPos.x()>=m_towersList[i]->m_pos.x()-21 && pressPos.y()<=m_towersList[i]->m_pos.y()+21 && pressPos.y()>=m_towersList[i]->m_pos.y()-21)
                     {
                       m_towersList[i]=new PurpleTower(it->centerPos(),this);
                       update();
                       break;
                      }
                  }
                  }
                  else if (!it->hasRedTower() && it->hasPurpleTower())
                  {
                      it->setHasRedTower();
                      for(int i=0;i<m_towersList.size();i++)
                      {
                           if(pressPos.x()<=m_towersList[i]->m_pos.x()+21 && pressPos.x()>=m_towersList[i]->m_pos.x()-21 && pressPos.y()<=m_towersList[i]->m_pos.y()+21 && pressPos.y()>=m_towersList[i]->m_pos.y()-21)
                      {
                      m_towersList[i]=new RedTower(it->centerPos(),this);
                      update();
                      break;
                      }
                   }
                   }
                   update();
                   break;
               }
        ++it;
           }
    }
    else if(m_waves == 1){
        QPoint pressPos = event->pos();
        auto it = m_towerPositionsList1.begin();//按照塔的安放坐标遍历
        while (it != m_towerPositionsList1.end())
        {
            if (canBuyTower() && it->containPoint(pressPos))
            {
                m_audioPlayer->playSound(TowerPlaceSound);
                m_playrGold -= TowerCost;
                if (!it->hasBlueTower())
                {
                    it->setHasBlueTower();
                    BlueTower *tower = new BlueTower(it->centerPos(), this);
                    m_towersList.push_back(tower);
                }
                else if (!it->hasPurpleTower() && it->hasBlueTower())
                {
                     it->setHasPurpleTower();
                     for(int i=0;i<m_towersList.size();i++)
                     {
                        if(pressPos.x()<=m_towersList[i]->m_pos.x()+21 && pressPos.x()>=m_towersList[i]->m_pos.x()-21 && pressPos.y()<=m_towersList[i]->m_pos.y()+21 && pressPos.y()>=m_towersList[i]->m_pos.y()-21)
                         {
                           m_towersList[i]=new PurpleTower(it->centerPos(),this);
                           update();
                           break;
                          }
                      }
                      }
                      else if (!it->hasRedTower() && it->hasPurpleTower())
                      {
                          it->setHasRedTower();
                          for(int i=0;i<m_towersList.size();i++)
                          {
                               if(pressPos.x()<=m_towersList[i]->m_pos.x()+21 && pressPos.x()>=m_towersList[i]->m_pos.x()-21 && pressPos.y()<=m_towersList[i]->m_pos.y()+21 && pressPos.y()>=m_towersList[i]->m_pos.y()-21)
                          {
                          m_towersList[i]=new RedTower(it->centerPos(),this);
                          update();
                          break;
                          }
                       }
                       }
                       update();
                       break;
                   }
            ++it;
               }
    }
    else {
        QPoint pressPos = event->pos();
        auto it = m_towerPositionsList2.begin();//按照塔的安放坐标遍历
        while (it != m_towerPositionsList2.end())
        {
            if (canBuyTower() && it->containPoint(pressPos))
            {
                m_audioPlayer->playSound(TowerPlaceSound);
                m_playrGold -= TowerCost;
                if (!it->hasBlueTower())
                {
                    it->setHasBlueTower();
                    BlueTower *tower = new BlueTower(it->centerPos(), this);
                    m_towersList.push_back(tower);
                }
                else if (!it->hasPurpleTower() && it->hasBlueTower())
                {
                     it->setHasPurpleTower();
                     for(int i=0;i<m_towersList.size();i++)
                     {
                        if(pressPos.x()<=m_towersList[i]->m_pos.x()+21 && pressPos.x()>=m_towersList[i]->m_pos.x()-21 && pressPos.y()<=m_towersList[i]->m_pos.y()+21 && pressPos.y()>=m_towersList[i]->m_pos.y()-21)
                         {
                           m_towersList[i]=new PurpleTower(it->centerPos(),this);
                           update();
                           break;
                          }
                      }
                      }
                      else if (!it->hasRedTower() && it->hasPurpleTower())
                      {
                          it->setHasRedTower();
                          for(int i=0;i<m_towersList.size();i++)
                          {
                               if(pressPos.x()<=m_towersList[i]->m_pos.x()+21 && pressPos.x()>=m_towersList[i]->m_pos.x()-21 && pressPos.y()<=m_towersList[i]->m_pos.y()+21 && pressPos.y()>=m_towersList[i]->m_pos.y()-21)
                          {
                          m_towersList[i]=new RedTower(it->centerPos(),this);
                          update();
                          break;
                          }
                       }
                       }
                       update();
                       break;
                   }
            ++it;
               }
}
}

bool MainWindow::canBuyTower() const
{
	if (m_playrGold >= TowerCost)
		return true;
	return false;
}

void MainWindow::drawWave(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(400, 5, 100, 25), QString("WAVE : %1").arg(m_waves + 1));
}

void MainWindow::drawHP(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(30, 5, 100, 25), QString("HP : %1").arg(m_playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(200, 5, 200, 25), QString("GOLD : %1").arg(m_playrGold));
}

void MainWindow::doGameOver()
{
	if (!m_gameEnded)
	{
		m_gameEnded = true;
		// 此处应该切换场景到结束场景
		// 暂时以打印替代,见paintEvent处理
	}
}

void MainWindow::awardGold(int gold)
{
	m_playrGold += gold;
	update();
}

AudioPlayer *MainWindow::audioPlayer() const
{
	return m_audioPlayer;
}

void MainWindow::addWayPoints()
{
    //list
    WayPoint *wayPoint1 = new WayPoint(QPoint(440, 140));
    m_wayPointsList.push_back(wayPoint1);

    WayPoint *wayPoint2 = new WayPoint(QPoint(400, 140));
    m_wayPointsList.push_back(wayPoint2);
    wayPoint2->setNextWayPoint(wayPoint1);

    WayPoint *wayPoint3 = new WayPoint(QPoint(400, 210));
    m_wayPointsList.push_back(wayPoint3);
    wayPoint3->setNextWayPoint(wayPoint2);


    WayPoint *wayPoint4 = new WayPoint(QPoint(200, 210));
    m_wayPointsList.push_back(wayPoint4);
    wayPoint4->setNextWayPoint(wayPoint3);

    WayPoint *wayPoint5 = new WayPoint(QPoint(200, 70));
    m_wayPointsList.push_back(wayPoint5);
    wayPoint5->setNextWayPoint(wayPoint4);

    WayPoint *wayPoint6 = new WayPoint(QPoint(35, 70));
    m_wayPointsList.push_back(wayPoint6);
    wayPoint6->setNextWayPoint(wayPoint5);

    //list1
    WayPoint *_wayPoint1 = new WayPoint(QPoint(440,300));
    m_wayPointsList1.push_back(_wayPoint1);

    WayPoint *_wayPoint2 = new WayPoint(QPoint(230, 300));
    m_wayPointsList1.push_back(_wayPoint2);
    _wayPoint2->setNextWayPoint(_wayPoint1);

    WayPoint *_wayPoint3 = new WayPoint(QPoint(230, 170));
    m_wayPointsList1.push_back(_wayPoint3);
    _wayPoint3->setNextWayPoint(_wayPoint2);

    WayPoint *_wayPoint4 = new WayPoint(QPoint(280, 170));
    m_wayPointsList1.push_back(_wayPoint4);
    _wayPoint4->setNextWayPoint(_wayPoint3);

    WayPoint *_wayPoint5 = new WayPoint(QPoint(280, 120));
    m_wayPointsList1.push_back(_wayPoint5);
    _wayPoint5->setNextWayPoint(_wayPoint4);

    WayPoint *_wayPoint6 = new WayPoint(QPoint(170, 120));
    m_wayPointsList1.push_back(_wayPoint6);
    _wayPoint6->setNextWayPoint(_wayPoint5);

    WayPoint *_wayPoint7 = new WayPoint(QPoint(170, 70));
    m_wayPointsList1.push_back(_wayPoint7);
    _wayPoint7->setNextWayPoint(_wayPoint6);

    WayPoint *_wayPoint8 = new WayPoint(QPoint(50, 70));
    m_wayPointsList1.push_back(_wayPoint8);
    _wayPoint8->setNextWayPoint(_wayPoint7);

    //list2
    WayPoint *__wayPoint1 = new WayPoint(QPoint(60, 220));
    m_wayPointsList2.push_back(__wayPoint1);

    WayPoint *__wayPoint2 = new WayPoint(QPoint(60, 180));
    m_wayPointsList2.push_back(__wayPoint2);
    _wayPoint2->setNextWayPoint(__wayPoint1);

    WayPoint *__wayPoint3 = new WayPoint(QPoint(430, 180));
    m_wayPointsList2.push_back(__wayPoint3);
    __wayPoint3->setNextWayPoint(__wayPoint2);

    WayPoint *__wayPoint4 = new WayPoint(QPoint(430, 130));
    m_wayPointsList2.push_back(__wayPoint4);
    __wayPoint4->setNextWayPoint(__wayPoint3);

    WayPoint *__wayPoint5 = new WayPoint(QPoint(380, 130));
    m_wayPointsList2.push_back(__wayPoint5);
    __wayPoint5->setNextWayPoint(__wayPoint4);

    WayPoint *__wayPoint6 = new WayPoint(QPoint(380, 30));
    m_wayPointsList2.push_back(__wayPoint6);
    __wayPoint6->setNextWayPoint(__wayPoint5);

    WayPoint *__wayPoint7 = new WayPoint(QPoint(60, 30));
    m_wayPointsList2.push_back(__wayPoint7);
    __wayPoint7->setNextWayPoint(__wayPoint6);

    WayPoint *__wayPoint8 = new WayPoint(QPoint(60, 130));
    m_wayPointsList2.push_back(__wayPoint8);
    __wayPoint8->setNextWayPoint(__wayPoint7);


    WayPoint *__wayPoint9 = new WayPoint(QPoint(320, 130));
    m_wayPointsList2.push_back(__wayPoint9);
    __wayPoint9->setNextWayPoint(__wayPoint8);


    WayPoint *__wayPoint10 = new WayPoint(QPoint(320, 80));
    m_wayPointsList2.push_back(__wayPoint10);
    __wayPoint10->setNextWayPoint(__wayPoint9);


    WayPoint *__wayPoint11 = new WayPoint(QPoint(220, 80));
    m_wayPointsList2.push_back(__wayPoint11);
    __wayPoint11->setNextWayPoint(__wayPoint10);
}

void MainWindow::getHpDamage(int damage/* = 1*/)
{
   m_audioPlayer->playSound(LifeLoseSound);
   m_playerHp -= damage;
	if (m_playerHp <= 0)
		doGameOver();
}

void MainWindow::removedEnemy(Enemy *enemy)
{
	Q_ASSERT(enemy);

	m_enemyList.removeOne(enemy);
	delete enemy;

	if (m_enemyList.empty())
	{
		++m_waves;
		if (!loadWave())
		{
			m_gameWin = true;
		}
	}
}

void MainWindow::removedBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.removeOne(bullet);
	delete bullet;
}

void MainWindow::addBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.push_back(bullet);
}

void MainWindow::updateMap()
{
	foreach (Enemy *enemy, m_enemyList)
		enemy->move();
    foreach (PurpleTower *tower, m_towersList)
         tower->checkEnemyInRange();
	update();
}

void MainWindow::preLoadWavesInfo()
{
	QFile file(":/config/Waves.plist");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition.plist");
		return;
	}

	PListReader reader;
	reader.read(&file);

	// 获取波数信息
	m_wavesInfo = reader.data();

	file.close();
}

bool MainWindow::loadWave()
{
	if (m_waves >= m_wavesInfo.size())
		return false;

    WayPoint *startWayPoint = NULL;

    if(m_waves == 0){
        startWayPoint = m_wayPointsList.back();
    }
    else if(m_waves == 1){
        startWayPoint = m_wayPointsList1.back();
    }
    else {
        startWayPoint = m_wayPointsList2.back();
    }

	QList<QVariant> curWavesInfo = m_wavesInfo[m_waves].toList();

	for (int i = 0; i < curWavesInfo.size(); ++i)
	{
		QMap<QString, QVariant> dict = curWavesInfo[i].toMap();
		int spawnTime = dict.value("spawnTime").toInt();

		Enemy *enemy = new Enemy(startWayPoint, this);

        if(m_waves == 1)
            enemy->levelUp1();
        else if (m_waves == 2) {
            enemy->levelUp2();
        }

		m_enemyList.push_back(enemy);

		QTimer::singleShot(spawnTime, enemy, SLOT(doActivate()));
	}

    m_towersList.clear();

	return true;
}

QList<Enemy *> MainWindow::enemyList() const
{
	return m_enemyList;
}

void MainWindow::gameStart()
{
	loadWave();
}

void MainWindow::new_custom()
{
    m_waves = 0;
    ui->toolButton->close();
    ui->toolButton_2->close();
    ui->toolButton_3->close();
    addWayPoints();
    gameStart();
}

void MainWindow::new_custom1()
{
    m_waves = 1;
    ui->toolButton->close();
    ui->toolButton_2->close();
    ui->toolButton_3->close();
    addWayPoints();
    gameStart();
}

void MainWindow::new_custom2()
{
    m_waves = 2;
    ui->toolButton->close();
    ui->toolButton_2->close();
    ui->toolButton_3->close();
    addWayPoints();
    gameStart();
}
