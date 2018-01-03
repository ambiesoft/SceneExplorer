#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Ambiesoft");
    QCoreApplication::setOrganizationDomain("ambiesoft.fam.cx");
    QCoreApplication::setApplicationName("Scene Explorer");

    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
}
