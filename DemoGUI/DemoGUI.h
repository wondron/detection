#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DemoGUI.h"
#include <../WGraphics/widget/wgraphicswidget.h>
#include "qtextedit.h"
#include "smthingOpt.h"

class DemoGUI : public QMainWindow
{
    Q_OBJECT

public:
    DemoGUI(QWidget *parent = nullptr);
    ~DemoGUI();





signals:
    void operate();

    void on_showMsg(const QString& msg);

public:
    static QTextEdit* m_textEdit;

private:
    Ui::DemoGUIClass ui;
    Graphics::WGraphicsWidget* m_widget;
    smthingOpt* m_oprator;
};
