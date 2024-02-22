#pragma once

#include <QFrame>
#include <QGraphicsView>
#include "wgraphicsview.h"
#include "wgraphicsscene.h"
#include "../wgraphics_global.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QToolButton;
class QSlider;
QT_END_NAMESPACE

namespace Graphics
{
class WGraphicsView;
class WViewPrivate;

class WGRAPHICS_EXPORT WGraphicsWidget : public QFrame
{
    Q_OBJECT
public:
    enum{FITBTN, ZOOMIN, ZOOMOUT, CENTERON, ORIGIN, PRINTER, OPENGL, ANTIALIASE, LABELSHOW, AIMSWITCH};
    explicit WGraphicsWidget(const QString &name = "", QWidget *parent = nullptr);

    ~WGraphicsWidget();

    /**
     * @brief 获取view窗口
     */
    WGraphicsView *view() const;

    /**
     * @brief 获取窗口名称
     */
    QString widgetName() const;

    /**
     * @brief 切换当前的scene
     */
    void setCurrentScene(WGraphicsScene* scene);

    /**
     * @brief 返回当前scene指针
     */
    WGraphicsScene* currentScene() const;
    /**
     * @brief 更新标注，工具栏，Aim的位置
     */
    void updatePos();

    /**
     * @brief 显示标注信息。
     */
    void setLabelShow(const bool enabel);

    /**
     * @brief 显示瞄准信息
     */
    void setAimShow(const bool enabel);

    /**
     * @brief 工具栏显示
     */
    void setPanelShow(const bool enabel);

    /**
     * @brief 设置显示图像
     */
    void setImage(const QImage &);

    /**
     * @brief 获取当前显示图像
     */
    QImage getImage() const;

signals:
    void sendEvent(QEvent* e);

private slots:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void btnClicked();

public slots:
    void fpsChange(int num);
    void setWidgetName(QString);

private:
    void iniPanel();
    void initPanelStyle();
    void initScaleWidget();
    void initAimWidget();
    void initFpsLabel();
    void initMousePos();

private:
    WViewPrivate* d;
    QString iconPath = "";
};

}
