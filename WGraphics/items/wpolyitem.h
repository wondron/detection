#ifndef CWRITEPENITEM_H
#define CWRITEPENITEM_H
#include "QGraphicsItem"
#include "QMouseEvent"
#include "aiitembase.h"

namespace Graphics {

using namespace  std;
class WPolyItemPrivate;

class WPolyItem : public WAiItemBase
{

public:
    WPolyItem();
    ~WPolyItem() {}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

    //item作用域操作
    QRectF boundingRect() const;
    virtual void setBoundingRect(qreal a, qreal b);
    virtual void setBoundingRect(const QPointF &a, const QPointF &b);
    virtual void changeBoundingRect();

    //点集操作
    int setPoints(const QVector<QPointF> &pts);
    int setPoints(const QVector<QVector<qreal>> &pts);
    int setPoints(const QVector<QVector<QPointF>> &pts);
    void getPoints(QVector<QPointF> &pts);
    void getPoints(QVector<QVector<QPointF>> &pts);
    void getPoints(QVector<QVector<qreal>>   &pts);
    QVector<QPointF> qvectorPoints();
    bool isEmpty();

    //线宽操作
    QVector<int> widList();
    QVector<qreal> widListF();
    void setLineWidth(const int wid);

    //线框操作
    void setDrawRectLenth(const int lenth);
    void setDrawrect(bool a);

    //整体信息操作
    QVector<QVector<QVector<qreal>>> itemInfo();
    void setItemInfo(QVector<QVector<QVector<qreal>>> info);

    //其他操作
    void turnoffSwitch();
    bool getDrawRectSwitch();
    qreal caculateCrossLengthPtToLine(const QPointF &pt1, const QPointF &pt2, const QPointF pt3);
    bool pnpoly(QPointF testPoint, QVector<QPointF> &pointList);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QMouseEvent *event);

private:
    static QColor s_globalColor;
    WPolyItemPrivate *d;
};

}
#endif // CWRITEPENITEM_H
