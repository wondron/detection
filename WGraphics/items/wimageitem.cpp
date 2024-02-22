#include "wimageitem.h"
#include <QPainter>
#include <QDebug>
#include <chrono>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QCoreApplication>
#include <QEvent>
#include <QTimer>

namespace Graphics {

    class CGraphicsImageItemEvent : public QEvent
    {
    public:
        enum { Type = QEvent::MaxUser - 66 };
        CGraphicsImageItemEvent(const QImage& image)
            : QEvent(QEvent::Type(Type)), m_image(image) {}

        QImage image() const
        {
            return m_image;
        }

    private:
        QImage m_image;

    };

    WImageItem::WImageItem(QGraphicsItem* parent)
        : QGraphicsObject(parent)
    {
    }

    WImageItem::~WImageItem()
    {
    }

    QImage WImageItem::image() const
    {
        return m_image;
    }

    void WImageItem::setImage(const QImage& image)
    {
        qApp->postEvent(this, new CGraphicsImageItemEvent(image));
    }

    bool WImageItem::event(QEvent* ev)
    {
        if (ev->type() == CGraphicsImageItemEvent::Type) {
            auto img = static_cast<CGraphicsImageItemEvent*>(ev)->image();
            bool flag = (m_image.size() != img.size()) || m_image.isNull();
            m_image = img;
            fpsNum++;

            if (flag) {
                scene()->setSceneRect(boundingRect());
                emit imgSizeDif();
            }

            update();
            return true;
        }

        //if (ev->type() == QEvent::MouseMove) {
        //    if (m_image.isNull())
        //        return false;

        //    QPointF scenePos = mapToScene(static_cast<QGraphicsSceneMouseEvent*>(ev)->pos());
        //    int x = static_cast<int>(scenePos.x());
        //    int y = static_cast<int>(scenePos.y());

        //    qDebug() << "Mouse move - Pixel position: (" << x << "," << y << ")";
        //    return false;
        //}

        return QGraphicsObject::event(ev);
    }

    QPointF WImageItem::offset() const
    {
        return  m_offset;
    }

    void WImageItem::setOffset(const QPointF& offset)
    {
        if (m_offset == offset)return;
        prepareGeometryChange();
        m_offset = offset;
        update();
    }

    QRectF WImageItem::boundingRect() const
    {
        if (m_image.isNull())
            return QRectF(-200, -200, 400, 400);

        return QRectF(m_offset, m_image.size() / m_image.devicePixelRatioF());
    }

    void WImageItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        if (m_image.isNull())
            return;

        painter->drawImage(option->exposedRect, m_image, m_image.rect(), Qt::NoFormatConversion);
    }
}
