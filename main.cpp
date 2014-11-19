#include "pilesgui.h"

#include <QApplication>
#include <QDebug>
#include <QDirIterator>

int main(int argc, char *argv[])
{
    int ret_v = -1;
    QApplication a(argc, argv);

    // prepare the ground for easy access to settings
    QCoreApplication::setOrganizationName("Piles");
    QCoreApplication::setOrganizationDomain("piles.org");
    QCoreApplication::setApplicationName("PilesGui");

    Q_INIT_RESOURCE (appres);

    /*
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        qDebug() << it.next();
    }
    */
    PilesGui pg;
    if (pg.start ()) {
        ret_v = a.exec();
        pg.stop ();
    }

    Q_CLEANUP_RESOURCE(appres);

    return ret_v;
}
