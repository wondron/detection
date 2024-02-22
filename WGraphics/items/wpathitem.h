#ifndef CPATHITEM_H
#define CPATHITEM_H

#include <QGraphicsItem>
#include <QMouseEvent>
#include <QAction>
#include "QMenu"
#include "aiitembase.h"

namespace Graphics {

using namespace std;

class WPathItemPrivate;

class WPathItem: public WAiItemBase
{
    Q_OBJECT

public:
    WPathItem();
    WPathItem(QColor &);
    ~WPathItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //item区域操作
    QRectF boundingRect() const;
    virtual void setBoundingRect(qreal a, qreal b);
    virtual void setBoundingRect(const QPointF &a, const QPointF &b);
    virtual void changeBoundingRect();

    //点集操作
    QVector<QVector<QPointF>> points();
    QVector<QVector<qreal>> pointList();

    void setPoints(QVector<QVector<QPointF>>);
    void setPoints(QVector<QVector<qreal>>);

    bool isEmpty();

    //颜色设置
    static void setGlobalColor(QColor &);

    //item宽度设置
    QVector<int> widList();
    QVector<qreal> widListF();
    void setLinesWidth(QVector<int>);
    void setLinesWidthF(QVector<qreal>);

    //画笔宽度设置
    void setMaxPenWidth(const int width);
    int getMaxPenWidth() const;
    void setPenWidth(int width = 10);
    int penWidth() const;

    //橡皮擦设置
    void setEraserWidth(int width = 10);
    int eraserWidth() const;

    //整体操作
    QVector<QVector<QVector<qreal>>> itemInfo();
    void setItemInfo(QVector<QVector<QVector<qreal>>> info);

    //其他操作
    void itemClear();
    void deleteData(const QPointF &);
    bool isDrawMode();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QMouseEvent *e);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

signals:
    void itemCleared();
    void colorChanged(QColor &);
    void cleared();

public slots:
    void allBigger();
    void allSmaller();
    void actionUpper();
    void actionDown();
    void actionButton();
    void actionTop();
    void setDrawMode();
    void setEraserMode();
    void setMoveMode();

private:
    void initialMenu();

private:
    WPathItemPrivate *d;
    static QColor s_globalColor;
};

}

#endif // CPATHITEM_H
