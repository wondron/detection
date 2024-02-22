#include "wpolyitem.h"
#include "QPainter"
#include "QGraphicsSceneMouseEvent"
#include "QCursor"
#include "QMenu"
#include <QInputDialog>
#include <qdebug.h>
#include "QColorDialog"
#include "QMessageBox"
#include "QDebug"
#include "qgraphicsscene.h"

using namespace Graphics;

#define PI 3.1415926

#define UPDATE_RETURN \
update();\
return QGraphicsItem::mousePressEvent(event);\

QColor WPolyItem::s_globalColor = QColor(255, 0, 0);

class Graphics::WPolyItemPrivate
{
public:
    WPolyItemPrivate()
    {
        m_ptLetfTp = QPointF(0, 0);
        m_ptRgtBtn = QPointF(9000, 9000);
    }
    ~WPolyItemPrivate() {}
    int num = 0;
    QVector<QPointF> m_pts;
    int m_lineWidth = 2;
    int m_drawRectLen = 2;
    int mcurrentIndex = 0;
    bool isLeft;
    bool isMoveButton = false;
    bool m_drawSwitch = false;
    bool pointMoveSwitch = false;
    bool m_drawRectSwitch = true;

    QPointF m_ptLetfTp;
    QPointF m_ptRgtBtn;

    QPointF m_movePoint;
    QPointF m_hoverDrawPnt;
};

WPolyItem::WPolyItem(): d(new WPolyItemPrivate())
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsPanel);
    setAcceptHoverEvents(true);
    d->m_lineWidth = 1;
    d->isLeft = false;
    d->m_drawSwitch = true;
}

void WPolyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, false);
    QColor m_color = color();

    QPen pen(m_color, d->m_lineWidth);
    QBrush brush(m_color);
    pen.setCosmetic(true);
    if (isSelected()) m_color.setAlpha(40);
    brush.setColor(m_color);

    painter->setPen(pen);
    painter->setBrush(brush);
    QPainterPath startPath;
    if (d->m_pts.size() && d->m_drawSwitch) {
        startPath.moveTo(d->m_pts[0]);
        for (auto j : d->m_pts) startPath.lineTo(j);
        startPath.lineTo(d->m_hoverDrawPnt);
        painter->drawPath(startPath);
    }

    if (d->m_drawSwitch == false) {
        int size = d->m_pts.size();
        QPointF pointss[2000];
        for (int i = 0; i < size; i++) pointss[i] = d->m_pts.at(i);
        painter->drawPolygon(pointss, size);
    }

    pen.setColor(QColor(0, 0, 255));
    brush.setColor(QColor(0, 0, 0, 0));
    painter->setPen(pen);
    painter->setBrush(brush);
    if (d->m_drawRectSwitch) {
        for (auto i : d->m_pts){
            //painter->drawEllipse();
            int a = d->m_drawRectLen;
            painter->drawRect(QRect(i.x() - a, i.y() - a, 2 * a, 2 * a));
        }
    }
}

QRectF WPolyItem::boundingRect() const
{
    return QRectF(d->m_ptLetfTp, d->m_ptRgtBtn);
}

void WPolyItem::setBoundingRect(qreal a, qreal b)
{
    d->m_ptLetfTp = QPointF(0, 0);
    d->m_ptRgtBtn = QPointF(a, b);
}

void WPolyItem::setBoundingRect(const QPointF &a, const QPointF &b)
{
    d->m_ptLetfTp = a;
    d->m_ptRgtBtn = b;
}

void WPolyItem::changeBoundingRect()
{
    qreal aa = 99999;
    qreal bb = 99999;
    qreal cc = 0;
    qreal dd = 0;

    for (auto j : d->m_pts) {
        if (j.x() < aa) aa = j.x();
        if (j.y() < bb) bb = j.y();
        if (j.x() > cc) cc = j.x();
        if (j.y() > dd) dd = j.y();
    }

    setBoundingRect(QPointF(aa - 30, bb - 30), QPointF(cc + 30, dd + 30));
    update();
}

int WPolyItem::setPoints(const QVector<QPointF> &pts)
{
    d->m_pts.clear();

    for (auto point : pts)
        d->m_pts.push_back(point);

    turnoffSwitch();
    changeBoundingRect();

    return 0;
}

int WPolyItem::setPoints(const QVector<QVector<qreal>> &points)
{
    QPointF dian;
    d->m_pts.clear();
    for (int i = 0; i < points[0].size(); i++)
        if (i % 2 == 0)
            dian.setX(points[0][i]);
        else {
            dian.setY(points[0][i]);
            d->m_pts.push_back(dian);
        }
    turnoffSwitch();
    changeBoundingRect();

    return 0;
}

int WPolyItem::setPoints(const QVector<QVector<QPointF>> &pts)
{
    QVector<QPointF> dian = pts[0];

    if (!dian.size())
        return 1;

    d->m_pts.swap(dian);
    return 0;
}

void WPolyItem::getPoints(QVector<QPointF> &pts)
{
    pts.clear();
    pts = d->m_pts;
}

void WPolyItem::getPoints(QVector<QVector<QPointF> > &pts)
{
    pts.clear();
    pts.push_back(d->m_pts);
}

void WPolyItem::getPoints(QVector<QVector<qreal> > &pts)
{
    pts.clear();
    QVector<qreal> qrealpts;
    for( auto pt : d->m_pts){
        qrealpts.push_back(pt.x());
        qrealpts.push_back(pt.y());
    }
    pts.push_back(qrealpts);
}

QVector<QPointF> WPolyItem::qvectorPoints()
{
    QVector<QPointF> points;
    for (auto i : d->m_pts)
        points.push_back(i);
    return points;
}

bool WPolyItem::isEmpty()
{
    return d->m_pts.size();
}

QVector<int> WPolyItem::widList()
{
    QVector<int> color;
    color.push_back(d->m_lineWidth);
    return color;
}

QVector<qreal> WPolyItem::widListF()
{
    QVector<qreal> wid;
    wid.push_back((qreal)d->m_lineWidth);
    return wid;
}

void WPolyItem::setLineWidth(const int wid)
{
    d->m_lineWidth = wid;
}

void WPolyItem::setDrawRectLenth(const int lenth)
{
    if(lenth > 15 || lenth < 1)
        return;
    d->m_drawRectLen = lenth;
}

void WPolyItem::setDrawrect(bool a)
{
    d->m_drawRectSwitch = a;
}

void WPolyItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->scenePos();
    bool skip = false;
    static QPointF currentPoint;

    switch (event->button()) {
    //右键结束
    case Qt::RightButton:
        if (d->num == 0) {
            d->m_drawSwitch = false;
            d->m_pts.push_back(event->scenePos());
            setSelected(false);
            d->num++;
        }
        UPDATE_RETURN
    case Qt::LeftButton:
        d->isLeft = true;
        setBoundingRect(9999, 9999);
        if (!d->m_drawRectSwitch) {
            d->m_movePoint = QPointF(pos);
            d->isMoveButton = true;
            UPDATE_RETURN
        } else {
            break;
        }
    case Qt::MiddleButton:
        event->ignore();
        return;
    default: return;
    }

    if (d->m_drawSwitch) {
        d->m_pts.push_back(event->scenePos());
        UPDATE_RETURN
    }

    for (int i = 0; i < d->m_pts.size(); ++i) {
        if (QLineF(pos, d->m_pts[i]).length() < d->m_drawRectLen) {
            d->mcurrentIndex = i;
            d->pointMoveSwitch = true;
            skip = true;
            UPDATE_RETURN
        }

        int next = (i + 1) % d->m_pts.size();
        int lenth = d->m_drawRectLen;
        if ((pos.x() - d->m_pts[i].x()) * (pos.x() - d->m_pts[next].x()) <= 0) {
            if (caculateCrossLengthPtToLine(d->m_pts[i], d->m_pts[next], pos) < lenth &&
                    QLineF(pos, d->m_pts[i]).length() > 2 * lenth &&
                    QLineF(pos, d->m_pts[next]).length() > 2 *lenth) {
                currentPoint = QPointF(pos);
                d->m_pts.insert(next, currentPoint);
                d->mcurrentIndex = next;
                d->pointMoveSwitch = true;
                UPDATE_RETURN
            }
        }
    }

    if (pnpoly(pos, d->m_pts)) {
        d->m_movePoint = QPointF(pos);
        d->isMoveButton = true;
    }
    UPDATE_RETURN
}

void WPolyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    d->isLeft = false;
    d->pointMoveSwitch = false;
    d->isMoveButton = false;
    if (event->button() == Qt::MidButton) return;
    if (d->m_drawSwitch)
        d->m_hoverDrawPnt = event->pos();
    if (event->button() == Qt::RightButton || !d->m_drawSwitch || d->num) {
        changeBoundingRect();
        setSelected(false);
    }

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void WPolyItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!d->isLeft) return QGraphicsItem::mouseMoveEvent(event);
    if (d->pointMoveSwitch) {
        prepareGeometryChange();
        d->m_pts[d->mcurrentIndex] = QPointF(event->scenePos());
    }

    if (d->isMoveButton) {
        QPointF changeSize = event->pos() - d->m_movePoint;
        QVector<QPointF> pints;
        for (auto i : d->m_pts) pints.push_back(changeSize + i);
        d->m_pts.clear();
        d->m_pts.swap(pints);
        d->m_movePoint = event->pos();
    }
    update();
    QGraphicsItem::mouseMoveEvent(event);
}

void WPolyItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    QColor color = QColorDialog::getColor(s_globalColor);
    if (color.isValid()) {
        setColor(color);
        s_globalColor = color;
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void WPolyItem::wheelEvent(QMouseEvent *e)
{
    if (e->buttons()&Qt::MiddleButton) e->accept();
}

void WPolyItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
}

void WPolyItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

void WPolyItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (d->m_drawSwitch)
        d->m_hoverDrawPnt = event->scenePos();
    update();
    QGraphicsItem::hoverMoveEvent(event);
}

qreal WPolyItem::caculateCrossLengthPtToLine(const QPointF &pt1, const QPointF &pt2, const QPointF pt3)
{
    if (pt1.x() == pt2.x()) {
        return abs(pt3.x() - pt1.x());
    } else if (pt1.y() == pt2.y()) {
        return abs(pt3.y() - pt1.y());
    } else {
        qreal angle = QLineF(pt1, pt2).angleTo(QLineF(pt1, pt3));
        qreal leng = sin(angle / 180 * PI) * (QLineF(pt1, pt3).length());
        return abs(leng);
    }
}

bool WPolyItem::pnpoly(QPointF testPoint, QVector<QPointF> &pointList)
{
    float vertx[99];
    float verty[99];
    for (int i = 0; i < pointList.size(); i++) {
        vertx[i] = pointList[i].x();
        verty[i] = pointList[i].y();
    }
    int nvert = d->m_pts.size();
    float testx = testPoint.x();
    float testy = testPoint.y();
    int i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if ( ((verty[i] > testy) != (verty[j] > testy)) &&
                (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]) )
            c = !c;
    }
    return c;
}

QVector<QVector<QVector<qreal>>> WPolyItem::itemInfo()
{
    QVector<QVector<QVector<qreal>>> info;
    QVector<QVector<qreal>> points;
    getPoints(points);

    QVector<QVector<qreal>> data;
    data.push_back(colorListF());
    data.push_back(widListF());
    data.push_back(getZValueList());

    info.push_back(points);
    info.push_back(data);
    return info;
}

void WPolyItem::setItemInfo(QVector<QVector<QVector<qreal>>> info)
{
    if (!info.size()) return;

    setPoints(info[0]);

    for (int i = 0; i < info[1].size(); i++) {
        switch (i) {
        case 0:
            setColor(info[1][0]);
            break;
        case 2:
            setZValueList(info[1][2]);
            break;
        }
    }
}

void WPolyItem::turnoffSwitch()
{
    d->m_drawSwitch = false;
}

bool WPolyItem::getDrawRectSwitch()
{
    return d->m_drawRectSwitch;
}
