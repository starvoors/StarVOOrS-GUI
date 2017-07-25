#include "starvoorsGUI.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StarvoorsGUI w;
    w.show();

    return a.exec();
}
