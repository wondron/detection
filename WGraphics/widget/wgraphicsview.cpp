#include "wgraphicsview.h"
#include "QWheelEvent"
#include "QMouseEvent"
#include "QScrollBar"
#include "QDebug"
#include <QHBoxLayout>
#include "QTime"
#include "QFontDatabase"
#include "QPushButton"
#include "QDir"
#include "QApplication"
#include "QPixmap"
#include "QPrinter"
#include "qprintdialog.h"

#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif

#define ICONSIZE 30

namespace Graphics {
WGraphicsView::WGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    QTransform matrix;
    setTransform(matrix.scale(1, 1));
    m_timer = new QTimer();
    setRenderHint(QPainter::Antialiasing, false);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //installEventFilter(this);

    QPixmap pixmap(40, 40);
    QPainter painter(&pixmap);
    painter.fillRect(0, 0, 20, 20, QColor(235, 235, 235));
    painter.fillRect(20, 20, 20, 20, QColor(235, 235, 235));
    painter.fillRect(20, 0, 20, 20, QColor(255, 255, 255));
    painter.fillRect(0, 20, 20, 20, QColor(255, 255, 255));
    setBackgroundBrush(QBrush(pixmap));
}

void WGraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->angleDelta().y() > 0)
            zoomIn(1.05);
        else
            zoomOut(0.95);
    } else {
        QGraphicsView::wheelEvent(e);
    }
}

void WGraphicsView::zoomIn(qreal level)
{
    qreal scale = transform().m11();
    scale *= level;
    if (scale < 0.001) return;
    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix);
    emit scaleChanged(scale);
}

void WGraphicsView::zoomOut(qreal level)
{
    qreal scale = transform().m11();
    scale *= level;
    if (scale < 0.001) return;
    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix);
    emit scaleChanged(scale);
}

void WGraphicsView::zoom(qreal scale)
{
    if (scale < 0.001) return;

    qreal s = transform().m11();
    if(s == scale) return;

    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix);
    emit scaleChanged(scale);
}

void WGraphicsView::fitScene()
{
    if (scene()) {
        auto rw = viewport()->size().width() / scene()->sceneRect().width();
        auto rh = viewport()->size().height() / scene()->sceneRect().height();
        auto sc  = qMin(rw, rh);

        if (qAbs(transform().m11() - sc) < 0.001) return;
        QTransform tr;
        tr.scale(sc, sc);
        setTransform(tr);
        update();
    }
}

void WGraphicsView::movetoCenter()
{
    if (!scene()) return;
    centerOn(scene()->sceneRect().center());
}

void WGraphicsView::print()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printdialog)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        render(&painter);
    }
#endif
}

void WGraphicsView::setOpenGl(bool enabel)
{
#ifndef QT_NO_OPENGL
    setViewport(enabel ? new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
}

void WGraphicsView::setAntialiase(bool enabel)
{
    setRenderHint(QPainter::Antialiasing, enabel);
}

void WGraphicsView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton) {
        dragFlag = true;
        setDragMode(QGraphicsView::ScrollHandDrag);
        //发生中键的按时，返回的还是左键。 第一个left是触发鼠标事件的按键，第二个是触发事件时，鼠标的状态
        QMouseEvent evt(e->type(), e->localPos(), e->windowPos(), e->screenPos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, e->source());
        QGraphicsView::mousePressEvent(&evt);
    }
    QGraphicsView::mousePressEvent(e);
}

void WGraphicsView::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton) {
        dragFlag = false;
        setDragMode(QGraphicsView::NoDrag);
    }
    QGraphicsView::mouseReleaseEvent(e);
}

void WGraphicsView::mouseMoveEvent(QMouseEvent *e)
{
    if (dragFlag) {
        emit dragChanged();
    }

    if (scene()) {
        // Convert mouse position to scene coordinates
        QPointF scenePos = mapToScene(e->pos());
        emit posMoved(scenePos.x(), scenePos.y());
    }

    QGraphicsView::mouseMoveEvent(e);
}

void WGraphicsView::keyPressEvent(QKeyEvent *e)
{
    QGraphicsView::keyPressEvent(e);
}

void WGraphicsView::keyReleaseEvent(QKeyEvent *e)
{
    QGraphicsView::keyReleaseEvent(e);
}

bool WGraphicsView::event(QEvent *e)
{
    return QGraphicsView::event(e);
}
}
