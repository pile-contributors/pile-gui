#include "pilesgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    int ret_v = -1;
    QApplication a(argc, argv);

    // prepare the ground for easy access to settings
    QCoreApplication::setOrganizationName("Piles");
    QCoreApplication::setOrganizationDomain("piles.org");
    QCoreApplication::setApplicationName("PilesGui");

    PilesGui pg;
    if (pg.start ()) {
        ret_v = a.exec();
        pg.stop ();
    }
    return ret_v;
}
