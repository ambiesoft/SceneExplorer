#include <QThread>
#include <QProcess>
#include <QIODevice>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Close_triggered()
{
    this->close();
}

bool getDuration(double& d)
{
    QProcess process;
    process.setProgram("C:\\LegacyPrograms\\ffmpeg\\bin\\ffprobe.exe");
    process.setArguments( QStringList() <<
                          "C:\\T\\input.flv" <<
                          "-hide_banner" <<
                          "-show_format" <<
                          "-print_format" <<
                          "json"
                          );

    process.start(QProcess::ReadOnly);
    if(!process.waitForStarted())
        return false;

    if(!process.waitForFinished())
        return false;

    qDebug() << "exitCode()=" << process.exitCode();

    QByteArray baOut = process.readAllStandardOutput();
    qDebug()<<baOut.data();

    QByteArray baErr=process.readAllStandardError();
    qDebug() << baErr.data();


    QJsonDocument jd = QJsonDocument::fromJson(baOut);
    if(jd.isNull())
        return false;

    QJsonObject sett2 = jd.object();
    QJsonValue format = sett2.value(QString("format"));
    qWarning() << format;
    QJsonObject item = format.toObject();
    qWarning() << QObject::tr("QJsonObject of description: ") << item;

    /* in case of string value get value and convert into string*/
    qWarning() << QObject::tr("QJsonObject[appName] of description: ") << item["duration"];
    QJsonValue duration = item["duration"];
    qWarning() << duration.toString();

    if(!duration.isString())
        return false;

    QString ds = duration.toString();

    bool ok;
    d = ds.toDouble(&ok);
    return ok;
}
void a()
{
    double d;
    if(!getDuration(d))
        return;

    for(int i=1 ; i <=5 ;++i)
    {
        QString filename="thumb";
        filename.append(QString::number(i));
        filename.append(".png");

        int timepoint = (int)(((double)i-0.5)*d/5);
        QStringList qsl;
        qsl <<
               "-ss" <<
               QString::number(timepoint) <<
               "-i" <<
               "C:\\T\\input.flv" <<
               "-vf" <<
               "select='eq(pict_type\\,I)'"<<
               "-vframes" <<
               "1"<<
               filename;
        QProcess ffmpeg;
        ffmpeg.setProgram("C:\\LegacyPrograms\\ffmpeg\\bin\\ffmpeg.exe");
        ffmpeg.setArguments(qsl);
        ffmpeg.start(QProcess::ReadOnly);

        if(!ffmpeg.waitForStarted())
            return;

        if(!ffmpeg.waitForFinished())
            return;

        qDebug() << "exitCode()=" << ffmpeg.exitCode();

        QByteArray baOut = ffmpeg.readAllStandardOutput();
        qDebug()<<baOut.data();

        QByteArray baErr=ffmpeg.readAllStandardError();
        qDebug() << baErr.data();
    }
}
void MainWindow::on_action_Do_It_triggered()
{
    QThread* pThread = QThread::create(a);
    pThread->start();
    pThread->wait();
}
