#ifndef TOWERPOSITION_H
#define TOWERPOSITION_H

#include <QPoint>
#include <QSize>
#include <QPixmap>

class QPainter;

class TowerPosition
{
public:
	TowerPosition(QPoint pos, const QPixmap &sprite = QPixmap(":/image/open_spot.png"));

    void setHasBlueTower(bool hasTower = true);
    void setHasPurpleTower(bool hasTower = true);
    void setHasRedTower(bool hasTower = true);
    bool hasBlueTower() const;
    bool hasPurpleTower() const;
    bool hasRedTower() const;
	const QPoint centerPos() const;
	bool containPoint(const QPoint &pos) const;

	void draw(QPainter *painter) const;

private:
    bool		m_hasBlueTower;
    bool		m_hasPurpleTower;
    bool		m_hasRedTower;
	QPoint		m_pos;
	QPixmap		m_sprite;

	static const QSize ms_fixedSize;
};

#endif // TOWERPOSITION_H
