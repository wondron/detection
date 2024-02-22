/********************************************************************************
** Form generated from reading UI file 'aiwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AIWIDGET_H
#define UI_AIWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AiWidget
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_open;
    QPushButton *btn_save;
    QPushButton *btn_saveOne;
    QPushButton *btn_pen;
    QPushButton *btn_polygon;
    QPushButton *btn_delete;
    QPushButton *btn_eraser;
    QPushButton *btn_help;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label;
    QComboBox *cmbx_colorSlct;
    QLabel *label_pen;
    QSlider *m_hSliderpen;
    QLabel *label_eraser;
    QSlider *m_hSlidereraser;
    QCheckBox *chkbx_autoSave;
    QCheckBox *chkbx_preSeg;
    QPushButton *btn_trainModel;
    QFrame *line;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *grpBx_model;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_modelSlct;
    QLineEdit *lineEdit;
    QPushButton *btn_model;
    QFrame *line_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *btn_back;
    QListWidget *listWidget;
    QPushButton *btn_next;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *hlay_src;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *hlay_label;
    QFrame *line_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *AiWidget)
    {
        if (AiWidget->objectName().isEmpty())
            AiWidget->setObjectName(QString::fromUtf8("AiWidget"));
        AiWidget->resize(1101, 554);
        centralwidget = new QWidget(AiWidget);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, 3, -1);
        btn_open = new QPushButton(centralwidget);
        btn_open->setObjectName(QString::fromUtf8("btn_open"));
        btn_open->setMinimumSize(QSize(30, 30));
        btn_open->setFlat(true);

        horizontalLayout_2->addWidget(btn_open);

        btn_save = new QPushButton(centralwidget);
        btn_save->setObjectName(QString::fromUtf8("btn_save"));
        btn_save->setMinimumSize(QSize(30, 30));
        btn_save->setFlat(true);

        horizontalLayout_2->addWidget(btn_save);

        btn_saveOne = new QPushButton(centralwidget);
        btn_saveOne->setObjectName(QString::fromUtf8("btn_saveOne"));
        btn_saveOne->setMinimumSize(QSize(30, 30));
        btn_saveOne->setFlat(true);

        horizontalLayout_2->addWidget(btn_saveOne);

        btn_pen = new QPushButton(centralwidget);
        btn_pen->setObjectName(QString::fromUtf8("btn_pen"));
        btn_pen->setMinimumSize(QSize(30, 30));
        btn_pen->setFlat(true);

        horizontalLayout_2->addWidget(btn_pen);

        btn_polygon = new QPushButton(centralwidget);
        btn_polygon->setObjectName(QString::fromUtf8("btn_polygon"));
        btn_polygon->setMinimumSize(QSize(30, 30));
        btn_polygon->setFlat(true);

        horizontalLayout_2->addWidget(btn_polygon);

        btn_delete = new QPushButton(centralwidget);
        btn_delete->setObjectName(QString::fromUtf8("btn_delete"));
        btn_delete->setMinimumSize(QSize(30, 30));
        btn_delete->setFlat(true);

        horizontalLayout_2->addWidget(btn_delete);

        btn_eraser = new QPushButton(centralwidget);
        btn_eraser->setObjectName(QString::fromUtf8("btn_eraser"));
        btn_eraser->setMinimumSize(QSize(30, 30));
        btn_eraser->setFlat(true);

        horizontalLayout_2->addWidget(btn_eraser);

        btn_help = new QPushButton(centralwidget);
        btn_help->setObjectName(QString::fromUtf8("btn_help"));
        btn_help->setMinimumSize(QSize(30, 30));
        btn_help->setFlat(true);

        horizontalLayout_2->addWidget(btn_help);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(10);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, -1, -1, -1);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        label->setFont(font);

        horizontalLayout_7->addWidget(label);

        cmbx_colorSlct = new QComboBox(centralwidget);
        cmbx_colorSlct->setObjectName(QString::fromUtf8("cmbx_colorSlct"));
        cmbx_colorSlct->setFont(font);
        cmbx_colorSlct->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid gray;\n"
"    border-radius: 3px;\n"
"    padding: 1px 2px 1px 2px;\n"
"    background-color: rgb(230,230,230);\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"	background-color: rgb(180,180,180);\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 20px;\n"
"    border-left-width: 1px;\n"
"    border-left-color: darkgray;\n"
"    border-left-style: solid;\n"
"    border-top-right-radius: 3px; \n"
"    border-bottom-right-radius: 3px;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image: url(:/aiImageIcon/aiImgIcon/down.png);\n"
"}\n"
"\n"
"QComboBox QAbstractItemView\n"
"{\n"
"background-color:rgba(225, 225, 255, 180);\n"
"height:1000px;\n"
"border: 12px; \n"
"color: red; \n"
"selection-color: green;\n"
"selection-background-color: rgb(210,210,210); \n"
"}"));

        horizontalLayout_7->addWidget(cmbx_colorSlct);

        label_pen = new QLabel(centralwidget);
        label_pen->setObjectName(QString::fromUtf8("label_pen"));
        label_pen->setFont(font);
        label_pen->setStyleSheet(QString::fromUtf8("selection-color: rgb(85, 255, 0);"));

        horizontalLayout_7->addWidget(label_pen);

        m_hSliderpen = new QSlider(centralwidget);
        m_hSliderpen->setObjectName(QString::fromUtf8("m_hSliderpen"));
        m_hSliderpen->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(m_hSliderpen);

        label_eraser = new QLabel(centralwidget);
        label_eraser->setObjectName(QString::fromUtf8("label_eraser"));
        label_eraser->setFont(font);

        horizontalLayout_7->addWidget(label_eraser);

        m_hSlidereraser = new QSlider(centralwidget);
        m_hSlidereraser->setObjectName(QString::fromUtf8("m_hSlidereraser"));
        m_hSlidereraser->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(m_hSlidereraser);

        chkbx_autoSave = new QCheckBox(centralwidget);
        chkbx_autoSave->setObjectName(QString::fromUtf8("chkbx_autoSave"));
        chkbx_autoSave->setFont(font);

        horizontalLayout_7->addWidget(chkbx_autoSave);

        chkbx_preSeg = new QCheckBox(centralwidget);
        chkbx_preSeg->setObjectName(QString::fromUtf8("chkbx_preSeg"));
        chkbx_preSeg->setFont(font);

        horizontalLayout_7->addWidget(chkbx_preSeg);

        btn_trainModel = new QPushButton(centralwidget);
        btn_trainModel->setObjectName(QString::fromUtf8("btn_trainModel"));
        btn_trainModel->setMinimumSize(QSize(80, 0));
        btn_trainModel->setFont(font);
        btn_trainModel->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"background-color:rgb(230,230,230);\n"
"border-radius: 3px;\n"
"border: 1px groove rgb(80, 80, 80)\n"
"}\n"
"\n"
"QPushButton::hover{\n"
"background-color:rgb(0, 153, 255, 60);\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"background-color: rgb(0, 153, 255,120);\n"
"border-style: inset; \n"
"}"));

        horizontalLayout_7->addWidget(btn_trainModel);


        horizontalLayout_2->addLayout(horizontalLayout_7);


        verticalLayout_2->addLayout(horizontalLayout_2);

        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShadow(QFrame::Raised);
        line->setFrameShape(QFrame::HLine);

        verticalLayout_2->addWidget(line);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(7);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 0, -1, -1);
        grpBx_model = new QGroupBox(centralwidget);
        grpBx_model->setObjectName(QString::fromUtf8("grpBx_model"));
        horizontalLayout_6 = new QHBoxLayout(grpBx_model);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_modelSlct = new QLabel(grpBx_model);
        label_modelSlct->setObjectName(QString::fromUtf8("label_modelSlct"));
        label_modelSlct->setFont(font);

        horizontalLayout_6->addWidget(label_modelSlct);

        lineEdit = new QLineEdit(grpBx_model);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_6->addWidget(lineEdit);

        btn_model = new QPushButton(grpBx_model);
        btn_model->setObjectName(QString::fromUtf8("btn_model"));
        btn_model->setFont(font);

        horizontalLayout_6->addWidget(btn_model);


        horizontalLayout_4->addWidget(grpBx_model);


        verticalLayout_2->addLayout(horizontalLayout_4);

        line_3 = new QFrame(centralwidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, -1, -1, -1);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        btn_back = new QPushButton(centralwidget);
        btn_back->setObjectName(QString::fromUtf8("btn_back"));
        btn_back->setMaximumSize(QSize(120, 16777215));
        btn_back->setFlat(true);

        verticalLayout->addWidget(btn_back);

        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy);
        listWidget->setMaximumSize(QSize(120, 16777215));

        verticalLayout->addWidget(listWidget);

        btn_next = new QPushButton(centralwidget);
        btn_next->setObjectName(QString::fromUtf8("btn_next"));
        btn_next->setMaximumSize(QSize(120, 16777215));
        btn_next->setFlat(true);

        verticalLayout->addWidget(btn_next);


        horizontalLayout->addLayout(verticalLayout);

        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(frame_2);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        hlay_src = new QHBoxLayout();
        hlay_src->setObjectName(QString::fromUtf8("hlay_src"));

        horizontalLayout_3->addLayout(hlay_src);


        horizontalLayout->addWidget(frame_2);

        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        hlay_label = new QHBoxLayout();
        hlay_label->setObjectName(QString::fromUtf8("hlay_label"));

        horizontalLayout_5->addLayout(hlay_label);


        horizontalLayout->addWidget(frame);


        verticalLayout_2->addLayout(horizontalLayout);

        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_2);

        AiWidget->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(AiWidget);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        AiWidget->setStatusBar(statusbar);

        retranslateUi(AiWidget);

        QMetaObject::connectSlotsByName(AiWidget);
    } // setupUi

    void retranslateUi(QMainWindow *AiWidget)
    {
        AiWidget->setWindowTitle(QCoreApplication::translate("AiWidget", "MainWindow", nullptr));
        btn_open->setText(QString());
        btn_save->setText(QString());
        btn_saveOne->setText(QString());
        btn_pen->setText(QString());
        btn_polygon->setText(QString());
        btn_delete->setText(QString());
        btn_eraser->setText(QString());
        btn_help->setText(QString());
        label->setText(QCoreApplication::translate("AiWidget", "\351\242\234\350\211\262\351\200\211\346\213\251", nullptr));
        label_pen->setText(QCoreApplication::translate("AiWidget", "\347\224\273\347\254\224\345\244\247\345\260\217:1", nullptr));
        label_eraser->setText(QCoreApplication::translate("AiWidget", "\346\251\241\347\232\256\346\223\246\345\244\247\345\260\217:1", nullptr));
        chkbx_autoSave->setText(QCoreApplication::translate("AiWidget", "\350\207\252\345\212\250\344\277\235\345\255\230", nullptr));
        chkbx_preSeg->setText(QCoreApplication::translate("AiWidget", "\351\242\204\346\240\207\346\263\250", nullptr));
        btn_trainModel->setText(QCoreApplication::translate("AiWidget", "\346\250\241\345\236\213\350\256\255\347\273\203", nullptr));
        grpBx_model->setTitle(QString());
        label_modelSlct->setText(QCoreApplication::translate("AiWidget", "\346\250\241\345\236\213\346\226\207\344\273\266\351\200\211\346\213\251", nullptr));
        btn_model->setText(QCoreApplication::translate("AiWidget", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        btn_back->setText(QString());
        btn_next->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AiWidget: public Ui_AiWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AIWIDGET_H
