#ifndef TOWER_H
#define TOWER_H

#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>


class QPainter;
class Enemy;
class MainWindow;
class QTimer;

class PurpleTower : QObject
{
    Q_OBJECT
public:
    PurpleTower(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower.png"));
    ~PurpleTower();
    QPoint getPos() const;
    void draw(QPainter *painter) const;
    void checkEnemyInRange();
    void targetKilled();
    void attackEnemy();
    void chooseEnemyForAttack(Enemy *enemy);
    void removeBullet();
    void damageEnemy();
    void lostSightOfEnemy();
    bool			m_attacking;
    int				m_attackRange;	// 代表塔可以攻击到敌人的距离
    int				m_damage;		// 代表攻击敌人时造成的伤害
    int				m_fireRate;		// 代表再次攻击敌人的时间间隔
    qreal			m_rotationSprite;

    Enemy *			m_chooseEnemy;
    MainWindow *	m_game;
    QTimer *		m_fireRateTimer;

    const QPoint	m_pos;
    const QPixmap	m_sprite;

    static const QSize ms_fixedSize;
protected slots:
    void shootWeapon();
};

class BlueTower : public PurpleTower
{
public:
    BlueTower(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower2.png")):PurpleTower(pos,game,sprite)
{
    m_fireRate = 400;
    m_damage=10;
    m_attackRange= 60;
}
    ~BlueTower();
};

class RedTower : public PurpleTower
{
public:
    RedTower(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower3.jpg")):PurpleTower(pos,game,sprite)
    {
        m_attackRange = 100;
        m_damage=30;
    }
    ~RedTower();
};

#endif // TOWER_H
