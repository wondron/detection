#pragma once
#include <QGraphicsScene>

namespace Graphics {

enum ZVALUE{MIN = 0, IMAGZVALUE = 800, MAX = 1500};

class WImageItem;
class WScenePrivate;

class WGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    WGraphicsScene(QObject* parent = Q_NULLPTR);
    virtual ~WGraphicsScene();

    /**
     * @brief 获取图像item
     */
    WImageItem* centerImageItem() const;

    /**
     * @brief 获取显示图像
     */
    QImage getImage() const;

    /**
     * @brief 辅助线使能
     */
    void  setCenterImageAimVisible(bool visible);

public slots:
    void setCenterImage(const QImage& image);

signals:
    void centerImageChanged();
    void imageSizeChange();
    void fpsChanged(int);

private:
    WScenePrivate* d;
};

}







