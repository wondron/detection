#include "DemoGUI.h"
#include <QtWidgets/QApplication>
#include <../WGraphics/widget/wgraphicswidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DemoGUI* widget = new DemoGUI();

    widget->show(); 

    return a.exec();
}
