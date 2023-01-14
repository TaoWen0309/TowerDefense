#include "towerposition.h"
#include <QPainter>

const QSize TowerPosition::ms_fixedSize(44, 44);

TowerPosition::TowerPosition(QPoint pos, const QPixmap &sprite/* = QPixmap(":/image/open_spot.png")*/)
    : m_hasBlueTower(false),
      m_hasPurpleTower(false),
      m_hasRedTower(false),
      m_pos(pos),
      m_sprite(sprite)
{
}

bool TowerPosition::hasBlueTower() const
{
    return m_hasBlueTower;
}

bool TowerPosition::hasPurpleTower() const
{
    return m_hasPurpleTower;
}

bool TowerPosition::hasRedTower() const
{
    return m_hasRedTower;
}

void TowerPosition::setHasBlueTower(bool hasTower/* = true*/)
{
    m_hasBlueTower = hasTower;
}

void TowerPosition::setHasPurpleTower(bool hasTower/* = true*/)
{
    m_hasPurpleTower = hasTower;
}

void TowerPosition::setHasRedTower(bool hasTower/* = true*/)
{
    m_hasRedTower = hasTower;
}

const QPoint TowerPosition::centerPos() const
{
    QPoint offsetPoint(ms_fixedSize.width()/2, ms_fixedSize.height()/2);
	return m_pos + offsetPoint;
}

bool TowerPosition::containPoint(const QPoint &pos) const
{
    bool isXInHere = m_pos.x() < pos.x() && pos.x() < (m_pos.x() + ms_fixedSize.width());
    bool isYInHere = m_pos.y() < pos.y() && pos.y() < (m_pos.y() + ms_fixedSize.height());
	return isXInHere && isYInHere;
}

void TowerPosition::draw(QPainter *painter) const
{
	painter->drawPixmap(m_pos.x(), m_pos.y(), m_sprite);
}
