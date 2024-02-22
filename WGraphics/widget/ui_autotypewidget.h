/********************************************************************************
** Form generated from reading UI file 'autotypewidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTOTYPEWIDGET_H
#define UI_AUTOTYPEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_autoTypeWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *vlayout;

    void setupUi(QWidget *autoTypeWidget)
    {
        if (autoTypeWidget->objectName().isEmpty())
            autoTypeWidget->setObjectName(QString::fromUtf8("autoTypeWidget"));
        autoTypeWidget->resize(689, 439);
        horizontalLayout = new QHBoxLayout(autoTypeWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        frame = new QFrame(autoTypeWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        vlayout = new QVBoxLayout();
        vlayout->setObjectName(QString::fromUtf8("vlayout"));

        horizontalLayout_2->addLayout(vlayout);


        horizontalLayout->addWidget(frame);


        retranslateUi(autoTypeWidget);

        QMetaObject::connectSlotsByName(autoTypeWidget);
    } // setupUi

    void retranslateUi(QWidget *autoTypeWidget)
    {
        autoTypeWidget->setWindowTitle(QCoreApplication::translate("autoTypeWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class autoTypeWidget: public Ui_autoTypeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTOTYPEWIDGET_H
