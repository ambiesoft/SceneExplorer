#include <map>

#include <QCoreApplication>
#include <QMap>
#include <QtGlobal>
#include <QTime>
#include <QElapsedTimer>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qsrand( QTime::currentTime().msec() );
    int MAX_ELEMENT_COUNT = 100000;
    QStringList allKey, allVal;
    for(int i=0; i < MAX_ELEMENT_COUNT ; ++i)
    {
        allKey.append(QString::number(qrand()));
        allVal.append(QString::number(qrand()));
    }

    QElapsedTimer timer;
    for(int loop=0; loop < 3;++loop)
    {
        timer.start();
        QMap<QString, QString> qmap;
        for(int i=0; i < MAX_ELEMENT_COUNT;++i)
            qmap[allKey[i]]=allVal[i];
        qDebug() << QString("QMap takes %1 msec").arg(timer.elapsed());

        timer.start();
        std::map<QString,QString> stdmap;
        for(int i=0; i < MAX_ELEMENT_COUNT;++i)
            stdmap[allKey[i]]=allVal[i];
        qDebug() << QString("std::map takes %1 msec").arg(timer.elapsed());
    }
    return 0;
}
