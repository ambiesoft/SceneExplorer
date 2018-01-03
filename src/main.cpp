#include <QApplication>
#include <QSettings>

#include "mainwindow.h"
#include "consts.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(Consts::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(Consts::APPDOMAIN);
    QCoreApplication::setApplicationName(Consts::APPNAME);

    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
}
