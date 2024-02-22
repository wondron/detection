#pragma once

#include <QGraphicsItem>
#include <QMouseEvent>
#include <QAction>
#include "QMenu"

namespace Graphics {
class WAiItemBase : public QObject, public QGraphicsItem
{
public:
    WAiItemBase();
    ~WAiItemBase() {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //item区域操作
    virtual void setBoundingRect(qreal, qreal) {}
    virtual void setBoundingRect(const QPointF &, const QPointF &) {}
    virtual void changeBoundingRect() {}

    //颜色设置
    void setColor(const QColor &color);
    void setColor(const QVector<qreal> &colorList);
    QColor color();
    QVector<int> colorList();
    QVector<qreal> colorListF();

    //ZValue List
    QVector<qreal> getZValueList();
    void setZValueList(const QVector<qreal> &zvalue);

private:
    //边界
    QPointF m_ptLetfTp = QPointF(0, 0);
    QPointF m_ptRgtBtn = QPointF(9000, 9000);
    QColor m_color;
};

}
