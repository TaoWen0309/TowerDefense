#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QPixmap>
#include "tower.h"

class WayPoint;
class QPainter;
class MainWindow;

class Enemy : public QObject
{
	Q_OBJECT
public:
    Enemy(WayPoint *startWayPoint, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/enemy333.png"));
	~Enemy();

	void draw(QPainter *painter) const;
	void move();
	void getDamage(int damage);
    void getRemoved();

    void getAttacked(BlueTower *attacker);
    void getAttacked(RedTower *attacker);
    void getAttacked(PurpleTower *attacker);

    void gotLostSight(BlueTower *attacker);
    void gotLostSight(RedTower *attacker);
    void gotLostSight(PurpleTower *attacker);

    void levelUp1();
    void levelUp2();
	QPoint pos() const;

public slots:
	void doActivate();

private:
	bool			m_active;
	int				m_maxHp;
	int				m_currentHp;
	qreal			m_walkingSpeed;
	qreal			m_rotationSprite;

	QPoint			m_pos;
	WayPoint *		m_destinationWayPoint;
	MainWindow *	m_game;
    QList<PurpleTower *>	m_PurpleTowerattackedTowersList;
    QList<BlueTower *>	m_BlueTowerattackedTowersList;
    QList<RedTower *>	m_RedTowerattackedTowersList;

	const QPixmap	m_sprite;
	static const QSize ms_fixedSize;
};

#endif // ENEMY_H
