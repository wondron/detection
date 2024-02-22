#include "aiitembase.h"
#include "QDebug"

Graphics::WAiItemBase::WAiItemBase()
{
    m_ptLetfTp = QPoint(0, 0);
    m_ptRgtBtn = QPointF(9000, 9000);
}

void Graphics::WAiItemBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void Graphics::WAiItemBase::setColor(const QColor &color)
{
    m_color = color;
}

void Graphics::WAiItemBase::setColor(const QVector<qreal> &colorList)
{
    if (colorList.size() < 3)
        return;
    m_color = QColor(colorList[0], colorList[1], colorList[2]);
}

QColor Graphics::WAiItemBase::color()
{
    return  m_color;
}

QVector<int> Graphics::WAiItemBase::colorList()
{
    QVector<int> color;
    color.push_back(m_color.red());
    color.push_back(m_color.green());
    color.push_back(m_color.blue());
    return color;
}

QVector<qreal> Graphics::WAiItemBase::colorListF()
{
    QVector<qreal> color;
    color.push_back((qreal)m_color.red());
    color.push_back((qreal)m_color.green());
    color.push_back((qreal)m_color.blue());
    return color;
}

QVector<qreal> Graphics::WAiItemBase::getZValueList()
{
    QVector<qreal> zz;
    zz.push_back(zValue());
    return zz;
}

void Graphics::WAiItemBase::setZValueList(const QVector<qreal> &zvalue)
{
    if (!zvalue.size()) return;
    setZValue(zvalue[0]);
}
