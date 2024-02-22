#include "DemoGUI.h"

DemoGUI::DemoGUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_widget = new Graphics::WGraphicsWidget();

    ui.vLayout->addWidget(m_widget);

}

DemoGUI::~DemoGUI()
{
    delete m_widget;
}
