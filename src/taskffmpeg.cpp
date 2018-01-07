#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>
#include <QDir>

#include "globals.h"
#include "consts.h"

#include "taskffmpeg.h"

TaskFFmpeg::TaskFFmpeg(int id,const QString& file)
{
    id_=id;
    movieFile_=file;

    progress_ = Uninitialized;
    // emit sayBorn(id,file);
}
TaskFFmpeg::~TaskFFmpeg()
{
    emit sayDead(id_);
}
bool getDuration(const QString& file,double& d,QString& videoFormat)
{
    QProcess process;
    process.setProgram("C:\\LegacyPrograms\\ffmpeg\\bin\\ffprobe.exe");
    process.setArguments( QStringList() <<
                          file <<
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
    if(0 != process.exitCode())
            return false;

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
    // qWarning() << item;

    QJsonValue format_name = item["format_name"];
    if(
            format_name.toString()=="tty" ||
            format_name.toString()=="image2"
      )
    {
        return false;
    }
    videoFormat = format_name.toString();

    /* in case of string value get value and convert into string*/
    // qWarning() << QObject::tr("QJsonObject[appName] of description: ") << item["duration"];
    QJsonValue duration = item["duration"];
    qWarning() << duration.toString();

    if(!duration.isString())
        return false;

    QString ds = duration.toString();

    bool ok;
    d = ds.toDouble(&ok);
    return ok;
}

void TaskFFmpeg::run()
{
    if(gStop)
        return;
    while(gPaused)
        QThread::sleep(5);
    if(gStop)
        return;

    progress_ = Progressing;
    emit sayHello(id_, movieFile_);
    if(!run2())
        emit sayNo(id_, movieFile_);
    progress_ = Finished;
}
bool TaskFFmpeg::run2()
{
    double d;
    QString format;
    if(!getDuration(movieFile_,d,format))
        return false;

    QString strWidthHeight;
    strWidthHeight.append(QString::number(Consts::THUMB_WIDTH));
    strWidthHeight.append("x");
    strWidthHeight.append(QString::number(Consts::THUMB_HEIGHT));

    QString thumbfile = QUuid::createUuid().toString();
    thumbfile = thumbfile.remove(L'{');
    thumbfile = thumbfile.remove(L'}');
    QStringList emitFiles;
    for(int i=1 ; i <=5 ;++i)
    {
        QString filename=thumbfile;
        filename.append("-");
        filename.append(QString::number(i));
        filename.append(".png");

        double timepoint = (((double)i-0.5)*d/5);
        QStringList qsl;
        qsl.append("-hide_banner");  // as it is
        qsl.append("-n");  // no overwrite
        qsl.append("-ss" );  // seek input
        qsl.append(QString::number(timepoint) );  // seek position
        qsl.append("-i" );  // input file
        qsl.append(movieFile_ );  // input file
        qsl.append("-vf" );  // video filtergraph
        qsl.append("select='eq(pict_type\\,I)'");  // select filter with argument, Select only I-frames:
        qsl.append("-vframes" );
        qsl.append("1");
        qsl.append("-s");
        qsl.append(strWidthHeight);
        qsl.append(QString(Consts::FILEPART_THUMBS) + QDir::separator() + filename);

        QProcess ffmpeg;
        ffmpeg.setProgram("C:\\LegacyPrograms\\ffmpeg\\bin\\ffmpeg.exe");
        ffmpeg.setArguments(qsl);
        ffmpeg.start(QProcess::ReadOnly);

        if(!ffmpeg.waitForStarted())
            return false;

        if(!ffmpeg.waitForFinished())
            return false;

        if(ffmpeg.exitCode() != 0)
            return false;

//        QByteArray baOut = ffmpeg.readAllStandardOutput();
//        qDebug()<<baOut.data();

//        QByteArray baErr=ffmpeg.readAllStandardError();
//        qDebug() << baErr.data();

        emitFiles.append(filename);
    }

    emit sayGoodby(id_,emitFiles, Consts::THUMB_WIDTH, Consts::THUMB_HEIGHT, movieFile_, format);
    return true;
}
