#ifndef WIMAGEITEM_H
#define WIMAGEITEM_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QImage>
#include <QTimer>

namespace Graphics {

class WImageItem: public QGraphicsObject
{
    Q_OBJECT
public:
    explicit WImageItem(QGraphicsItem *parennt = Q_NULLPTR);
    ~WImageItem() override;

    QImage image() const;

    QPointF offset() const;
    void setOffset(const QPointF &offset);
    inline void setOffset(qreal x, qreal y);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    void imgChanged();
    void imgSizeDif();
    void imgchgeFps(int);

public slots:
    void setImage(const QImage &image);

private:
    bool event(QEvent *ev) override;
    int fpsNum = 0;
    QImage m_image;
    QPointF m_offset;
};
}
#endif // WIMAGEITEM_H
