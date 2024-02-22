#pragma once
#include "QGraphicsItem"
#include "QObject"

namespace Graphics {

class WCircleItemPrivate;

class WCircleItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    enum {MIN = 1, LINEMAX = 10, MAX = 40};
    explicit WCircleItem(QPointF center = QPointF(100, 100), qreal radius = 20);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    void setCols(const int num);
    void setRows(const int num);
    int Col() const;
    int Row() const;

    QPointF center() const;
    void setcenter(const QPointF center);

    float radius() const;
    void  setRadius(const float radius);

    void setOutWidth(const int wid);
    void setGridWidth(const int wid);
    void setOutColor(const QColor clr);
    void setGridColor(const QColor clr);

    int outWidth() const;
    int gridWidth() const;
    QColor outColor() const;
    QColor gridColor() const;

    void mousePressEvent  (QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void radiusChange(float radius);

private:
    void setboundingRect(QPointF a, QPointF b);

private:
    WCircleItemPrivate *d;
};
}
