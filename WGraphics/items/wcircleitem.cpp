#include "wcircleitem.h"
#include "QPen"
#include "QPainter"
#include "QGraphicsSceneMouseEvent"
#include "QDebug"

namespace Graphics {

class WCircleItemPrivate
{
public:
    WCircleItemPrivate(WCircleItem *q) : qq(q)
    {

    }
    WCircleItem *qq;
    QPointF m_center;

    QPointF movePts;
    QPointF leftTop;
    QPointF rightBtm;

    bool moveSwitch;
    bool rChangeSwitch = false; //半径变化开关
    float m_radius;

    QColor m_outerColor = QColor(0, 255, 0);
    int m_outerWidth = 2;

    QColor m_gridColor = QColor(255, 0, 0);
    int m_gridWidth = 2;

    int m_cols = 1;
    int m_rows = 1;
};

WCircleItem::WCircleItem(QPointF center, qreal radius) :
    d(new WCircleItemPrivate(this))
{

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    d->m_center = center;
    d->m_radius = radius;
}

void WCircleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (d->m_rows < MIN
            || d->m_rows > MAX
            || d->m_cols < MIN
            || d->m_cols > MAX)
        return;

    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen(d->m_outerColor, d->m_outerWidth);
    pen.setCosmetic(true);
    painter->setPen(pen);
    float radius = d->m_radius;

    float x = d->m_center.x() - radius;
    float y = d->m_center.y() - radius;
    QPointF ltPts = QPointF(x, y);
    painter->drawEllipse(QRectF(ltPts, QSize( 2 * radius, 2 * radius)));

    pen.setWidth(d->m_gridWidth);
    pen.setColor(d->m_gridColor);
    painter->setPen(pen);
    painter->drawRect(QRectF(ltPts, QSize(2 * radius, 2 * radius)));

    QVector<QRectF> rects;
    float delX, delY, width, height;
    width = 2 * radius / d->m_cols;
    height = 2 * radius / d->m_rows;
    for (int i = 1 ; i <= d->m_rows ; i ++) {
        for ( int j = 1 ; j <= d->m_cols ; j++) {
            delX = j * 2 * radius / d->m_cols;
            delY = i * 2 * radius / d->m_rows;
            QPointF rightBtn = ltPts + QPointF(delX, delY);
            QRectF rect = QRectF(ltPts, rightBtn);
            rects.push_back(rect);
        }
    }
    painter->drawRects(rects);
    return;
}

QRectF WCircleItem::boundingRect() const
{
    return QRectF(d->m_center.x() - d->m_radius - 5,
                  d->m_center.y() - d->m_radius - 5,
                  2 * d->m_radius + 10,
                  2 * d->m_radius + 10);
}

void WCircleItem::setCols(const int num)
{
    if (num < MIN || num > MAX)
        return;
    d->m_cols = num;
}

void WCircleItem::setRows(const int num)
{
    if (num < MIN || num > MAX)
        return;
    d->m_rows = num;
}

int WCircleItem::Col() const
{
    return d->m_cols;
}

int WCircleItem::Row() const
{
    return d->m_rows;
}

QPointF WCircleItem::center() const
{
    return d->m_center;
}

void WCircleItem::setcenter(const QPointF center)
{
    d->m_center = center;
}

float WCircleItem::radius() const
{
    return d->m_radius;
}

void WCircleItem::setRadius(const float radius)
{
    if(radius < 0) return;
    d->m_radius = radius;
}

void WCircleItem::setOutWidth(const int wid)
{
    if(wid < MIN || wid > LINEMAX) return;
    d->m_outerWidth = wid;
}

void WCircleItem::setGridWidth(const int wid)
{
    if(wid < MIN || wid > LINEMAX) return;
    d->m_gridWidth = wid;
}

void WCircleItem::setOutColor(const QColor clr)
{
    d->m_outerColor = clr;
}

void WCircleItem::setGridColor(const QColor clr)
{
    d->m_gridColor = clr;
}

int WCircleItem::outWidth() const
{
    return d->m_outerWidth;
}

int WCircleItem::gridWidth() const
{
    return d->m_gridWidth;
}

QColor WCircleItem::outColor() const
{
    return d->m_outerColor;
}

QColor WCircleItem::gridColor() const
{
    return d->m_gridColor;
}

void WCircleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    int lenth = d->m_radius - QLineF(center(), event->pos()).length();

    if (lenth > 10) {
        d->moveSwitch = true;
        d->movePts = event->pos();
    }
    if (lenth < 9 && lenth > -9)
        d->rChangeSwitch = true;
}

void WCircleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    prepareGeometryChange();
    if ((!d->rChangeSwitch) && d->moveSwitch) {
        d->m_center = d->m_center - d->movePts + event->pos();
        d->movePts = event->pos();
    } else if (d->rChangeSwitch) {
        d->m_radius = abs(QLineF(center(), event->pos()).length());
        emit radiusChange(d->m_radius);
    }
    update();
}

void WCircleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    d->rChangeSwitch = false;
    d->moveSwitch = false;
}

}
