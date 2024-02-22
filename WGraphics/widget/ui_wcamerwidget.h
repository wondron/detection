/********************************************************************************
** Form generated from reading UI file 'wcamerwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WCAMERWIDGET_H
#define UI_WCAMERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WCamerWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *grpbx_Cam;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *hlay_Cam;
    QHBoxLayout *hlay_camoprt;
    QPushButton *btn_single;
    QPushButton *btn_continue;
    QPushButton *btn_saveImg;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_pamset;
    QFrame *line_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_exp;
    QSpinBox *spin_exp;
    QSlider *hSlider_exp;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_gain;
    QSpinBox *spin_gain;
    QSlider *hSlider_gain;
    QFrame *line;

    void setupUi(QWidget *WCamerWidget)
    {
        if (WCamerWidget->objectName().isEmpty())
            WCamerWidget->setObjectName(QString::fromUtf8("WCamerWidget"));
        WCamerWidget->resize(477, 476);
        verticalLayout_2 = new QVBoxLayout(WCamerWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, -1, 0, 0);
        grpbx_Cam = new QGroupBox(WCamerWidget);
        grpbx_Cam->setObjectName(QString::fromUtf8("grpbx_Cam"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        grpbx_Cam->setFont(font);
        grpbx_Cam->setFlat(true);
        verticalLayout = new QVBoxLayout(grpbx_Cam);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(grpbx_Cam);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        hlay_Cam = new QHBoxLayout();
        hlay_Cam->setObjectName(QString::fromUtf8("hlay_Cam"));

        horizontalLayout_5->addLayout(hlay_Cam);


        verticalLayout->addWidget(frame);

        hlay_camoprt = new QHBoxLayout();
        hlay_camoprt->setObjectName(QString::fromUtf8("hlay_camoprt"));
        hlay_camoprt->setContentsMargins(-1, -1, -1, 0);
        btn_single = new QPushButton(grpbx_Cam);
        btn_single->setObjectName(QString::fromUtf8("btn_single"));

        hlay_camoprt->addWidget(btn_single);

        btn_continue = new QPushButton(grpbx_Cam);
        btn_continue->setObjectName(QString::fromUtf8("btn_continue"));

        hlay_camoprt->addWidget(btn_continue);

        btn_saveImg = new QPushButton(grpbx_Cam);
        btn_saveImg->setObjectName(QString::fromUtf8("btn_saveImg"));

        hlay_camoprt->addWidget(btn_saveImg);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hlay_camoprt->addItem(horizontalSpacer);

        btn_pamset = new QPushButton(grpbx_Cam);
        btn_pamset->setObjectName(QString::fromUtf8("btn_pamset"));

        hlay_camoprt->addWidget(btn_pamset);


        verticalLayout->addLayout(hlay_camoprt);

        line_2 = new QFrame(grpbx_Cam);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_exp = new QLabel(grpbx_Cam);
        label_exp->setObjectName(QString::fromUtf8("label_exp"));
        label_exp->setMinimumSize(QSize(40, 0));
        label_exp->setFont(font);
        label_exp->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_exp);

        spin_exp = new QSpinBox(grpbx_Cam);
        spin_exp->setObjectName(QString::fromUtf8("spin_exp"));
        spin_exp->setMaximum(99999);

        horizontalLayout->addWidget(spin_exp);

        hSlider_exp = new QSlider(grpbx_Cam);
        hSlider_exp->setObjectName(QString::fromUtf8("hSlider_exp"));
        hSlider_exp->setStyleSheet(QString::fromUtf8("color: rgb(255, 170, 0);\n"
"selection-color: rgb(255, 255, 0);"));
        hSlider_exp->setMaximum(99999);
        hSlider_exp->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(hSlider_exp);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_gain = new QLabel(grpbx_Cam);
        label_gain->setObjectName(QString::fromUtf8("label_gain"));
        label_gain->setMinimumSize(QSize(40, 0));
        label_gain->setFont(font);
        label_gain->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(label_gain);

        spin_gain = new QSpinBox(grpbx_Cam);
        spin_gain->setObjectName(QString::fromUtf8("spin_gain"));
        spin_gain->setMaximum(99999);

        horizontalLayout_3->addWidget(spin_gain);

        hSlider_gain = new QSlider(grpbx_Cam);
        hSlider_gain->setObjectName(QString::fromUtf8("hSlider_gain"));
        hSlider_gain->setStyleSheet(QString::fromUtf8("color: rgb(24, 217, 255);"));
        hSlider_gain->setMaximum(99999);
        hSlider_gain->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(hSlider_gain);


        verticalLayout->addLayout(horizontalLayout_3);

        line = new QFrame(grpbx_Cam);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);


        verticalLayout_2->addWidget(grpbx_Cam);


        retranslateUi(WCamerWidget);
        QObject::connect(hSlider_exp, SIGNAL(valueChanged(int)), spin_exp, SLOT(setValue(int)));
        QObject::connect(spin_exp, SIGNAL(valueChanged(int)), hSlider_exp, SLOT(setValue(int)));
        QObject::connect(hSlider_gain, SIGNAL(valueChanged(int)), spin_gain, SLOT(setValue(int)));
        QObject::connect(spin_gain, SIGNAL(valueChanged(int)), hSlider_gain, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(WCamerWidget);
    } // setupUi

    void retranslateUi(QWidget *WCamerWidget)
    {
        WCamerWidget->setWindowTitle(QCoreApplication::translate("WCamerWidget", "Form", nullptr));
        grpbx_Cam->setTitle(QCoreApplication::translate("WCamerWidget", "\347\233\270\346\234\272\347\225\214\351\235\242", nullptr));
        btn_single->setText(QCoreApplication::translate("WCamerWidget", "\351\207\207\345\233\276", nullptr));
        btn_continue->setText(QCoreApplication::translate("WCamerWidget", "\350\277\236\347\273\255\351\207\207\345\233\276", nullptr));
        btn_saveImg->setText(QCoreApplication::translate("WCamerWidget", "\344\277\235\345\255\230\345\233\276\345\203\217", nullptr));
        btn_pamset->setText(QCoreApplication::translate("WCamerWidget", "\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label_exp->setText(QCoreApplication::translate("WCamerWidget", "\346\233\235\345\205\211", nullptr));
        label_gain->setText(QCoreApplication::translate("WCamerWidget", "\345\242\236\347\233\212", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WCamerWidget: public Ui_WCamerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WCAMERWIDGET_H
