#include <QApplication>
#include <QSettings>

#include "consts.h"
#include "settings.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(Consts::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(Consts::APPDOMAIN);
    QCoreApplication::setApplicationName(Consts::APPNAME);

    QApplication app(argc, argv);

    Settings settings;
    MainWindow w(nullptr, settings);
    w.show();


    return app.exec();
}
