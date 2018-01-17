#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QDir>

#include "globals.h"
#include "consts.h"

#include "taskffmpeg.h"

int TaskFFmpeg::waitMax_ = -1;

TaskFFmpeg::TaskFFmpeg(int loopId, int id,const QString& file)
{
    loopId_ = loopId;
    id_=id;
    movieFile_=file;

    progress_ = Uninitialized;
    // emit sayBorn(id,file);
}
TaskFFmpeg::~TaskFFmpeg()
{
    emit sayDead(loopId_,id_);
}
bool TaskFFmpeg::getProbe(const QString& file,
              double& outDuration,
              QString& outFormat,
              int& outBitrate,

              QString& outVideoCodec,
              QString& outAudioCodec,

              int& outVWidth,
              int& outVHeight,

              QString& errorReason)
{
    QProcess process;
    process.setProgram("C:\\LegacyPrograms\\ffmpeg\\bin\\ffprobe.exe");
    process.setArguments( QStringList() <<
                          "-v" <<
                          "quiet" <<
                          "-hide_banner" <<
                          "-show_format" <<
                          "-show_streams" <<
                          "-print_format" <<
                          "json" <<
                          file
                          );

    process.start(QProcess::ReadOnly);
    if(!process.waitForStarted(waitMax_))
    {
        errorReason = tr("waitForStarted failed");
        return false;
    }
    if(!process.waitForFinished(waitMax_))
    {
        errorReason = tr("waitForFinished failed");
        return false;
    }

    qDebug() << "exitCode()=" << process.exitCode();
    if(0 != process.exitCode())
    {
        errorReason = tr("existCode != 0");
        return false;
    }
    QByteArray baOut = process.readAllStandardOutput();
    QByteArray baErr=process.readAllStandardError();


    QJsonDocument jd = QJsonDocument::fromJson(baOut);
    if(jd.isNull())
    {
        errorReason = tr("QJsonDocument is Null");
        return false;
    }

    QJsonObject jo = jd.object();
    if(jo.isEmpty())
    {
        errorReason = tr("QJsonObject is Empty");
        return false;
    }

    // format,duration,bitrate
    {
        QJsonValue format = jo.value(QString("format"));
        QJsonObject item = format.toObject();

        QJsonValue format_name = item["format_name"];
        if( format_name.toString()=="tty" )
        {
            errorReason = tr("tty format ignored");
            return false;
        }
        outFormat = format_name.toString();

        QJsonValue bit_rate = item["bit_rate"];
        QString strBitrate = bit_rate.toString();
        if(!strBitrate.isEmpty())
        {
            outBitrate = strBitrate.toInt();
        }

        QJsonValue jDuration = item["duration"];

        if(!jDuration.isString())
        {
            errorReason = tr("duration is not a string");
            return false;
        }

        QString ds = jDuration.toString();

        bool ok;
        outDuration = ds.toDouble(&ok);
        if(!ok)
        {
            errorReason = tr("duration is not double");
            return false;
        }
    }

    // stream
    outVideoCodec.clear();
    outAudioCodec.clear();
    {
        QJsonValue streams = jo.value(QString("streams"));
        for(const QJsonValue& value : streams.toArray())
        {
            QJsonObject item = value.toObject();

            QString ctype = item["codec_type"].toString();
            if(outVideoCodec.isEmpty() && ctype=="video")
            {
                outVideoCodec = item["codec_name"].toString();
                outVWidth = item["coded_width"].toInt();
                outVHeight = item["coded_height"].toInt();
            }
            if(outAudioCodec.isEmpty() && ctype=="audio")
            {
                outAudioCodec = item["codec_name"].toString();
            }

            if(!outVideoCodec.isEmpty() && !outAudioCodec.isEmpty())
                break;
        }
    }
    if(outVideoCodec.isEmpty())
    {
        errorReason = tr("No video streams found");
        return false;
    }
    return true;
}
void TaskFFmpeg::run()
{
    run2();
    emit finished_FFMpeg(loopId_,id_);
}

void TaskFFmpeg::run2()
{
    if(gStop)
        return;
    while(gPaused)
        QThread::sleep(5);
    if(gStop)
        return;

    progress_ = Progressing;
    emit sayHello(loopId_,id_, movieFile_);
    QString errorReason;
    if(!run3(errorReason))
        emit sayNo(loopId_,id_, movieFile_, errorReason);
    progress_ = Finished;
}
bool TaskFFmpeg::run3(QString& errorReason)
{
    double duration;
    QString format;
    int bitrate=0;
    QString vcodec,acodec;
    int vWidth,vHeight;
    if(!getProbe(movieFile_,
                 duration,
                 format,
                 bitrate,
                 vcodec,
                 acodec,
                 vWidth,vHeight,
                 errorReason))
    {
        return false;
    }

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

        QString actualFile = QString(Consts::FILEPART_THUMBS) + QDir::separator() + filename;

        double timepoint = (((double)i-0.5)*duration/5);
        QStringList qsl;
        qsl.append("-v");
        qsl.append("16");  // only error output
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
        qsl.append(actualFile);

        QProcess ffmpeg;
        ffmpeg.setProgram("C:\\LegacyPrograms\\ffmpeg\\bin\\ffmpeg.exe");
        ffmpeg.setArguments(qsl);
        ffmpeg.start(QProcess::ReadOnly);

        if (!ffmpeg.waitForStarted(waitMax_))
		{
			errorReason = tr("ffmpeg.waitForStarted failed");
			return false;
		}

        if (!ffmpeg.waitForFinished(waitMax_))
		{
			errorReason = tr("ffmpeg.waitForFinished failed");
			return false;
		}

		if (ffmpeg.exitCode() != 0)
		{
			errorReason = tr("ffmpeg.exitCode() != 0");
			errorReason += "\n\n";
			QByteArray baErr=ffmpeg.readAllStandardError();
			errorReason += baErr.data();
			return false;
		}

//        QByteArray baOut = ffmpeg.readAllStandardOutput();
//        qDebug()<<baOut.data();


        if(i==1)
        {
			if (!QFile(actualFile).exists())
			{
				errorReason = tr("Failed to create thumbnail");
				QByteArray baErr = ffmpeg.readAllStandardError();
				QString strErr = baErr.data();
				if (!strErr.isEmpty())
				{
					errorReason += "\n\n";
					errorReason += strErr;
				}
				return false;
			}
        }
        emitFiles.append(filename);
    }

    // emit sayGoodby(id_,emitFiles, Consts::THUMB_WIDTH, Consts::THUMB_HEIGHT, movieFile_, format);
    emit sayGoodby(loopId_,id_,
                   emitFiles,
                   movieFile_,
                   Consts::THUMB_WIDTH,
                   Consts::THUMB_HEIGHT,
                   duration,
                   format,
                   bitrate,
                   vcodec,acodec,
                   vWidth,vHeight
                   );
    return true;
}
