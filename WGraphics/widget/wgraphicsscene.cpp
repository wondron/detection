#include <QTransform>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QPaintEngine>
#include <QTimer>

#include "wgraphicsscene.h"
#include "items/wimageitem.h"

namespace Graphics {
class WAimLineItem : public QGraphicsItem
{
public:
    WAimLineItem(QGraphicsItem *parent): QGraphicsItem(parent)
    {
        m_pen.setStyle(Qt::DashDotLine);
        m_pen.setColor(QColor(50, 200, 50));
        m_pen.setWidth(1);
        m_pen.setCosmetic(true);
        setZValue(ZVALUE::MAX);
    }

    virtual QRectF boundingRect() const override
    {
        return parentItem()->boundingRect();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override
    {
        painter->setPen(m_pen);
        auto r = parentItem()->boundingRect();
        auto c = r.center();
        painter->drawLine(QPointF(c.x(), r.top()), QPointF(c.x(), r.bottom()));
        painter->drawLine(QPointF(r.left(), c.y()), QPointF(r.right(), c.y()));
    }
private:
    QPen m_pen;
};

class WScenePrivate
{
public:
    WScenePrivate(WGraphicsScene *qq): q(qq)
    {
        centerImage = new WImageItem();
        centerImageAim = new WAimLineItem(centerImage);
        fpsTimer = new QTimer(q);
        fps = 0;
        count = 0;

        QObject::connect(fpsTimer, &QTimer::timeout, [this]() {
            if (fps != count) {
                fps = count;
                emit q->fpsChanged(fps);
            }
            count = 0;
        });
        fpsTimer->start(1000);
        q->addItem(centerImage);

        QObject::connect(q, &WGraphicsScene::sceneRectChanged, [this](const QRectF & rect) {
            Q_UNUSED(rect)
            q->setSceneRect(centerImage->boundingRect());
        });
    }

    ~WScenePrivate()
    {
        delete centerImage;
        delete fpsTimer;
        delete centerImageAim;
    }

    WGraphicsScene *q;
    WImageItem *centerImage;
    WAimLineItem *centerImageAim;
    QTimer *fpsTimer;
    int count;
    int fps;
};

WGraphicsScene::WGraphicsScene(QObject *parent) :
    QGraphicsScene(parent), d(new WScenePrivate(this))
{
    connect(d->centerImage, &WImageItem::imgSizeDif, [this]() {
        emit imageSizeChange();
    });

}

WGraphicsScene::~WGraphicsScene()
{
    delete d;
}

WImageItem *WGraphicsScene::centerImageItem() const
{
    return d->centerImage;
}

QImage WGraphicsScene::getImage() const
{
    return d->centerImage->image();
}

void WGraphicsScene::setCenterImageAimVisible(bool visible)
{
    d->centerImageAim->setVisible(visible);
}

void WGraphicsScene::setCenterImage(const QImage &image)
{
    d->centerImage->setImage(image);
    d->count ++;
    emit centerImageChanged();
}
}
