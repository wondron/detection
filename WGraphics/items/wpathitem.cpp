#include "wpathitem.h"
#include "QPainter"
#include "QGraphicsSceneMouseEvent"
#include "QCursor"
#include "QMenu"
#include <QInputDialog>
#include <qdebug.h>
#include "QColorDialog"
#include "QMessageBox"

QColor Graphics::WPathItem::s_globalColor = QColor(255, 0, 0, 255);
using namespace Graphics;

class Graphics::WPathItemPrivate
{
public:

    WPathItemPrivate()
    {
        menu = new QMenu();
        isLeft = false;
        m_penWidth = 1;
        m_eraserWidth = 5;
        cursorIn = true;
        m_ptLetfTp = QPointF(0, 0);
        m_ptRgtBtn = QPointF(9000, 9000);
        m_maxPenWid = 99;
    }

    ~WPathItemPrivate()
    {
        delete menu;
    }

    QMenu *menu;
    QAction *bigger;
    QAction *smaller;
    QAction *down;
    QAction *upper;
    QAction *button;
    QAction *top;
    QAction *draw;
    QAction *eraser;
    QAction *move;

    //擦除后生成的lines
    bool m_eraserSwitch;
    bool m_eraser = false;
    int  m_eraserWidth;
    QVector<QVector<QPointF>> m_erasedlines;
    QVector<int> deletePos;
    QPointF m_eraserpoint;                //eraser的返回point

    int m_penWidth;
    int m_maxPenWid;

    QPointF m_ptLetfTp;
    QPointF m_ptRgtBtn;

    QPointF m_movePoint;
    bool isLeft;
    bool m_drawSwitch;
    bool cursorIn;
    QPointF m_hoverDrawPnt;
    QVector<int> m_totalWidth;
    QVector<QPointF> m_points;
    QVector<QVector<QPointF>> m_lines;
    QRectF rectOld;
    bool m_paint = false;
};

WPathItem::WPathItem() : d(new WPathItemPrivate())
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsPanel);
    setAcceptHoverEvents(true);
    initialMenu();
    setDrawMode();
}

WPathItem::WPathItem(QColor &color): WPathItem()
{
    setColor(color);
}

WPathItem::~WPathItem()
{
    delete d;
}

void WPathItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    d->menu->exec(event->screenPos());
    QGraphicsItem::contextMenuEvent(event);
}

void WPathItem::setPoints(QVector<QVector<QPointF>> lines)
{
    d->m_lines.clear();
    d->m_lines.swap(lines);
    changeBoundingRect();
    update();
}

void WPathItem::setPoints(QVector<QVector<qreal>> lines)
{
    QPointF dian;
    QVector<QPointF> xian;
    QVector<QVector<QPointF>> mian;
    for (auto line : lines) {
        xian.clear();
        for (int i = 0; i < line.size(); i++)
            if (i % 2 == 0)
                dian.setX(line[i]);
            else {
                dian.setY(line[i]);
                xian.push_back(dian);
            }
        mian.push_back(xian);
    }
    d->m_lines.swap(mian);
}

bool WPathItem::isEmpty()
{
    for (QVector<QPointF> i : d->m_lines) {
        if (i.size()) return true;
    }
    return false;
}

void WPathItem::setLinesWidth(QVector<int> width)
{
    d->m_totalWidth.swap(width);
}

void WPathItem::setLinesWidthF(QVector<qreal> width)
{
    QVector<int> wid;
    for (auto i : width)
        wid.push_back((int)i);
    d->m_totalWidth.swap(wid);
}

void WPathItem::setMaxPenWidth(const int width)
{
    if (width < 1 || width > 99)
        return;
    d->m_maxPenWid = width;
}

int WPathItem::getMaxPenWidth() const
{
    return d->m_maxPenWid;
}

void WPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, false);

    QPen pen;//画橡皮擦图标
    pen.setStyle(Qt::SolidLine);//画画笔
    QColor pencolor = color();

    if (isSelected())
        pencolor.setAlpha(125);

    pen.setColor(pencolor);
    painter->setPen(pen);

    QPainterPath startPath[200];
    int k = 0;
    for (int i = 0; i < d->m_lines.size(); i++) {
        pen.setWidth(d->m_totalWidth[i]);
        painter->setPen(pen);
        if (d->m_lines[i].size() == 1) {
            painter->drawPoint(d->m_lines[i][0]);
            continue;
        }
        startPath[k].moveTo(d->m_lines[i][0]);
        for (auto j : d->m_lines[i]) {
            startPath[k].lineTo(j);
        }
        painter->drawPath(startPath[k]);
        k++;
    }

    if (d->m_points.size()) {
        pen.setWidth(d->m_penWidth);
        painter->setPen(pen);
        if (d->m_points.size() == 1) {
            painter->drawPoint(d->m_points[0]);
        } else {
            startPath[k].moveTo(d->m_points[0]);
            for (auto j : d->m_points) {
                startPath[k].lineTo(j);
            }
            painter->drawPath(startPath[k]);
        }
    }
}

QRectF WPathItem::boundingRect() const
{
    return QRectF(d->m_ptLetfTp, d->m_ptRgtBtn);
}

void WPathItem::setBoundingRect(qreal a, qreal b)
{
    d->m_ptLetfTp = QPointF(0, 0);
    d->m_ptRgtBtn = QPointF(a, b);
}

void WPathItem::setBoundingRect(const QPointF &a, const QPointF &b)
{
    d->m_ptLetfTp = a;
    d->m_ptRgtBtn = b;
}

void WPathItem::changeBoundingRect()
{
    if (d->m_lines.size() == 0) {
        setBoundingRect(QPointF(1, 1), QPointF(2, 2));
        return;
    }
    qreal aa = 9999999;
    qreal bb = 9999999;
    qreal cc = 0;
    qreal dd = 0;

    for (auto i : d->m_lines)
        for (auto j : i) {
            if (j.x() < aa) aa = j.x();
            if (j.y() < bb) bb = j.y();
            if (j.x() > cc) cc = j.x();
            if (j.y() > dd) dd = j.y();
        }
    d->rectOld = boundingRect();
    setBoundingRect(QPointF(aa - 50, bb - 50), QPointF(cc + 50, dd + 50));
    update();
}

void WPathItem::setEraserMode()
{
    d->m_drawSwitch = false;
    d->m_eraserSwitch = true;
    setCursor(Qt::SizeAllCursor);
}

void WPathItem::setMoveMode()
{
    d->m_drawSwitch = false;
    d->m_eraserSwitch = false;
    setCursor(Qt::OpenHandCursor);
}

void WPathItem::setGlobalColor(QColor &color)
{
    s_globalColor = color;
}

QVector<int> WPathItem::widList()
{
    return d->m_totalWidth;
}

QVector<qreal> WPathItem::widListF()
{
    QVector<qreal> widF;
    for (auto i : d->m_totalWidth)
        widF.push_back((qreal)i);
    return widF;
}

QVector<QVector<qreal>> WPathItem::pointList()
{
    QVector<QVector<qreal>> lines;
    QVector<qreal> points;
    for (auto line : d->m_lines) {
        points.clear();
        for (auto point : line) {
            points.push_back(point.x());
            points.push_back(point.y());
        }
        lines.push_back(points);
    }
    return lines;
}

QVector<QVector<QVector<qreal>>> WPathItem::itemInfo()
{
    QVector<QVector<QVector<qreal>>> info;
    QVector<QVector<qreal>> data;
    info.push_back(pointList());

    data.push_back(colorListF());
    data.push_back(widListF());
    data.push_back(getZValueList());
    info.push_back(data);

    return info;
}

void WPathItem::setItemInfo(QVector<QVector<QVector<qreal>>> info)
{
    if (!info.size())
        return;

    setPoints(info[0]);

    for (int i = 0; i < info[1].size(); i++) {
        switch (i) {
        case 0:
            setColor(info[1][0]);
            break;
        case 1:
            setLinesWidthF(info[1][1]);
            break;
        case 2:
            setZValueList(info[1][2]);
            break;
        default:
            break;
        }
    }
    changeBoundingRect();
}

void WPathItem::initialMenu()
{
    d->bigger = d->menu->addAction(u8"变粗");
    d->smaller = d->menu->addAction(u8"变细");
    d->down =  d->menu->addAction(u8"下降");
    d->upper =  d->menu->addAction(u8"上升");
    d->top =  d->menu->addAction(u8"置顶");
    d->button =  d->menu->addAction(u8"置底");
    d->move =  d->menu->addAction(u8"移动");
    connect(d->bigger, SIGNAL(triggered()), this, SLOT(allBigger()));
    connect(d->smaller, SIGNAL(triggered()), this, SLOT(allSmaller()));
    connect(d->down, SIGNAL(triggered()), this, SLOT(actionDown()));
    connect(d->upper, SIGNAL(triggered()), this, SLOT(actionUpper()));
    connect(d->top, SIGNAL(triggered()), this, SLOT(actionTop()));
    connect(d->button, SIGNAL(triggered()), this, SLOT(actionButton()));
    connect(d->move, SIGNAL(triggered()), this, SLOT(setMoveMode()));
}

bool WPathItem::isDrawMode()
{
    return d->m_drawSwitch;
}

void WPathItem::setDrawMode()
{
    d->m_drawSwitch = true;
    d->m_eraserSwitch = false;
    setCursor(Qt::PointingHandCursor);
}

QVector<QVector<QPointF>> WPathItem::points()
{
    return d->m_lines;
}

void WPathItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        d->m_eraserSwitch = false;
        d->m_drawSwitch = false;
        return;
    } else d->isLeft = true;

    d->m_points.clear();
    if (d->isLeft && d->m_drawSwitch) {
        setBoundingRect(99999, 99999);
        d->m_points.push_back(event->scenePos());
        d->m_paint = true;
    }
    if (d->isLeft && d->m_eraserSwitch) {
        setCursor(Qt::CrossCursor);
        deleteData(event->scenePos());
        d->m_eraserpoint = event->scenePos();
        d->m_eraser = true;
    }

    if (d->isLeft && !d->m_eraserSwitch && !d->m_drawSwitch) {
        setCursor(Qt::ClosedHandCursor);
        d->m_movePoint = event->scenePos();
    }

    update();
    QGraphicsItem::mousePressEvent(event);
}

void WPathItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    d->m_paint = false;
    d->m_eraser = false;
    d->isLeft = false;
    if (event->button() == Qt::MidButton) return;
    if ((event->button() == Qt::LeftButton) && d->m_eraserSwitch) setCursor(Qt::SizeAllCursor);

    if (d->m_points.size() == 1) d->m_points.clear();
    if (d->m_drawSwitch && d->m_points.size()) {
        d->m_lines.push_back(d->m_points);
        d->m_totalWidth.push_back(d->m_penWidth);
        d->m_points.clear();
        emit cleared();
    }
    changeBoundingRect();
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void WPathItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!d->isLeft) return;
    if (d->m_drawSwitch && d->m_paint) {
        if (d->m_penWidth > 30 && QLineF(event->scenePos(), d->m_points[d->m_points.size() - 1]).length() < d->m_penWidth / 10) return;
        if (QLineF(event->scenePos(), d->m_points[d->m_points.size() - 1]).length() < d->m_penWidth / 5) return;
        d->m_points.push_back(event->scenePos());
    }

    if (d->m_eraserSwitch && d->m_eraser) {
        deleteData(event->scenePos());
        d->m_eraserpoint = event->scenePos();
    }

    if (d->isLeft && !d->m_eraserSwitch && !d->m_drawSwitch) {
        setBoundingRect(99999, 99999);
        QPointF changeSize = event->pos() - d->m_movePoint;
        QVector<QPointF> points;
        QVector<QVector<QPointF>> lines;
        for (auto line : d->m_lines) {
            points.clear();
            for (auto point : line) points.push_back(changeSize + point);
            lines.push_back(points);
        }
        d->m_lines.clear();
        d->m_lines.swap(lines);
        d->m_movePoint = event->scenePos();
    }
    update();
    QGraphicsItem::mouseMoveEvent(event);
}

void WPathItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    QColor color = QColorDialog::getColor(s_globalColor);
    if (color.isValid()) {
        setColor(color);
        s_globalColor = color;
    }
}

void WPathItem::wheelEvent(QMouseEvent *e)
{
    if (e->buttons()&Qt::MiddleButton) e->accept();
}

void WPathItem::deleteData(const QPointF &pt)
{
    bool copyData = false;
    QVector<QPointF> addPoint;
    QVector<int> m_erasedWidth;
    d->m_erasedlines.clear();

    for (int i = 0; i < d->m_lines.size(); i++) {
        copyData = false;
        d->deletePos.clear();
        d->deletePos.push_back(-1);

        for (int j = 0; j < d->m_lines[i].size(); j++) {
            if (QLineF(pt, d->m_lines[i][j]).length() < d->m_eraserWidth) {
                d->deletePos.push_back(j);
                copyData = true;
            }
        }

        if (copyData) {
            d->deletePos.push_back((int)d->m_lines[i].size());
            for (int j = 0; j < d->deletePos.size() - 1; j++) {
                if ((d->deletePos[j + 1] - d->deletePos[j]) > 1) {
                    addPoint.clear();
                    for (int index = d->deletePos[j] + 1; index < d->deletePos[j + 1]; index++) {
                        addPoint.push_back(d->m_lines[i][index]);
                    }
                    d->m_erasedlines.push_back(addPoint);
                    m_erasedWidth.push_back(d->m_totalWidth[i]);
                }
            }
        } else {
            d->m_erasedlines.push_back(d->m_lines[i]);
            m_erasedWidth.push_back(d->m_totalWidth[i]);
        }
    }
    d->m_lines.clear();
    d->m_totalWidth.clear();

    d->m_lines = d->m_erasedlines;
    d->m_totalWidth = m_erasedWidth;
}

void WPathItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (d->m_drawSwitch) setCursor(Qt::PointingHandCursor);
    if (d->m_eraserSwitch) setCursor(Qt::SizeAllCursor);
    d->cursorIn = true;
    QGraphicsItem::hoverEnterEvent(event);
}

void WPathItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    d->cursorIn = false;
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

void WPathItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (d->m_drawSwitch)
        d->m_hoverDrawPnt = event->scenePos();
    else
        d->m_eraserpoint = event->scenePos();

    update();
    QGraphicsItem::hoverMoveEvent(event);
}

void WPathItem::allBigger()
{
    setBoundingRect(9000, 9000);
    for (int i = 0; i < d->m_totalWidth.size(); i++) {
        if (d->m_totalWidth[i] < getMaxPenWidth())
            d->m_totalWidth[i] += 1;
    }
    update();
    changeBoundingRect();
}

void WPathItem::allSmaller()
{
    setBoundingRect(9000, 9000);
    for (int i = 0; i < d->m_totalWidth.size(); i++) {
        if (d->m_totalWidth[i] > 1)
            d->m_totalWidth[i] -= 1;
    }
    update();
    changeBoundingRect();
}

void WPathItem::actionUpper()
{
    int a = zValue();
    if (++a > 900) a = 900;
    setZValue(a);
}

void WPathItem::actionDown()
{
    int a = zValue();
    if (--a < 800) a = 800;
    setZValue(a);
}

void WPathItem::actionTop()
{
    setZValue(900);
}

void WPathItem::actionButton()
{
    setZValue(800);
}

void WPathItem::setPenWidth(int width)
{
    if (width > 0 && width < 100)
        d->m_penWidth = width;
}

int WPathItem::penWidth() const
{
    return d->m_penWidth;
}

void WPathItem::setEraserWidth(int width)
{
    if (width > 0 && width < 100)
        d->m_eraserWidth = width;
}

int WPathItem::eraserWidth() const
{
    return d->m_eraserWidth;
}

void WPathItem::itemClear()
{
    d->m_lines.clear();
    d->m_points.clear();
    d->m_totalWidth.clear();
}
