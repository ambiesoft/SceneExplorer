#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include "task.h"

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

void TaskFFMpeg::run()
{
    double d;
    QString format;
    if(!getDuration(file_,d,format))
        return;
    // qDebug() << file_ << d;

    int width=240;
    int height=180;
    QString strWidthHeight;
    strWidthHeight.append(QString::number(width));
    strWidthHeight.append("x");
    strWidthHeight.append(QString::number(height));

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

        int timepoint = (int)(((double)i-0.5)*d/5);
        QStringList qsl;
        qsl.append("-n");
        qsl.append("-ss" );
        qsl.append(QString::number(timepoint) );
        qsl.append("-i" );
        qsl.append(file_ );
        qsl.append("-vf" );
        qsl.append("select='eq(pict_type\\,I)'");
        qsl.append("-vframes" );
        qsl.append("1");
        qsl.append("-s");
        qsl.append(strWidthHeight);
        qsl.append(filename);

        QProcess ffmpeg;
        ffmpeg.setProgram("C:\\LegacyPrograms\\ffmpeg\\bin\\ffmpeg.exe");
        ffmpeg.setArguments(qsl);
        ffmpeg.start(QProcess::ReadOnly);

        if(!ffmpeg.waitForStarted())
            return;

        if(!ffmpeg.waitForFinished())
            return;

        if(ffmpeg.exitCode() != 0)
            return;

//        QByteArray baOut = ffmpeg.readAllStandardOutput();
//        qDebug()<<baOut.data();

//        QByteArray baErr=ffmpeg.readAllStandardError();
//        qDebug() << baErr.data();

        emitFiles.append(filename);
    }

    emit sayGoodby(id_,emitFiles, width, height, file_, format);
}
