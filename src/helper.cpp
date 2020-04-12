//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <QDir>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QFileInfo>
#include <QDesktopServices>
#include <QSet>
#include <QCoreApplication>
#include <QLocale>
#include <QComboBox>
#include <QList>
#include <QCommandLineParser>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QSqlError>
#include <QPalette>
#include <QLineEdit>
#include <QDateTime>

#include "../../lsMisc/stdQt/stdQt.h"

#include "commandoption.h"
#include "consts.h"
#include "osd.h"
#include "helper.h"


using namespace Consts;
using namespace AmbiesoftQt;





QString normalizeFile(const QString& file)
{
    return QFileInfo(file).absoluteFilePath();
}
void nomalizeDirAndName(const QString& full, QString&dir, QString& name)
{
    QFileInfo fi(full);
    dir = normalizeDir(fi.absolutePath());
    name = fi.fileName();
}

static QString getBytecode1(char c)
{
    QString ret;
    switch( (c & 0xF0)>>4 )
    {
    case 0: ret+="0";break;
    case 1: ret+="1";break;
    case 2: ret+="2";break;
    case 3: ret+="3";break;
    case 4: ret+="4";break;
    case 5: ret+="5";break;
    case 6: ret+="6";break;
    case 7: ret+="7";break;
    case 8: ret+="8";break;
    case 9: ret+="9";break;
    case 0xa: ret+="a";break;
    case 0xb: ret+="b";break;
    case 0xc: ret+="c";break;
    case 0xd: ret+="d";break;
    case 0xe: ret+="e";break;
    case 0xf: ret+="f";break;
    default:
        Q_ASSERT(false);
    }
    switch(c & 0x0F)
    {
    case 0: ret+="0";break;
    case 1: ret+="1";break;
    case 2: ret+="2";break;
    case 3: ret+="3";break;
    case 4: ret+="4";break;
    case 5: ret+="5";break;
    case 6: ret+="6";break;
    case 7: ret+="7";break;
    case 8: ret+="8";break;
    case 9: ret+="9";break;
    case 0xa: ret+="a";break;
    case 0xb: ret+="b";break;
    case 0xc: ret+="c";break;
    case 0xd: ret+="d";break;
    case 0xe: ret+="e";break;
    case 0xf: ret+="f";break;
    default:
        Q_ASSERT(false);
    }

    Q_ASSERT(ret.size()==2);
    return ret;
}
static QString getBytecode(const char* pByte, int size)
{
    QString ret;
    for(int i=0; i< size; ++i)
    {
        ret += getBytecode1(pByte[i]);
    }
    return ret;
}

QString createSalient(const QString& file, const qint64& size)
{
    QFile f(file);
    if(!f.open(QIODevice::ReadOnly))
        return QString();

    qint64 readed=0;
    char buff[20]={0};
    readed=f.read(buff, 20);
    if(readed <= 0)
        return QString();

    QString ret = getBytecode(buff, readed);

    if(size <= 20)
        return ret;

    if(!f.seek(size-20))
        return QString();
    readed=f.read(buff, 20);
    if(readed != 20)
        return QString();
    ret += "-";
    ret += getBytecode(buff, 20);

    if(size <= 40)
        return ret;

    if(!f.seek(size/3))
        return QString();
    readed=f.read(buff, 20);
    if(readed != 20)
        return QString();
    ret += "-";
    ret += getBytecode(buff, 20);

    if(!f.seek( size/3*2))
        return QString();
    readed=f.read(buff, 20);
    ret += "-";
    ret += getBytecode(buff, 20);

    return ret;
}



QString getUUIDFromThumbfile(const QString& file)
{
    int i = file.lastIndexOf('.');
    if(i < 0)
        return QString();
    QString ret = file.left(i);
    if(ret.length() < 2)
        return QString();
    ret = ret.left(ret.length()-2);
    return ret;
}


Qt::WindowFlags GetDefaultDialogFlags()
{
    return ((Qt::WindowTitleHint | Qt::WindowCloseButtonHint| Qt::WindowFlags()) & ~Qt::WindowContextHelpButtonHint);
}
void setReadOnly(QLineEdit& le)
{
    QPalette palette;
    palette.setColor(QPalette::Base,Qt::gray);
    palette.setColor(QPalette::Text,Qt::black);
    le.setPalette(palette);
}

QString GetSortColumnName(SORTCOLUMNMY sc)
{
    switch (sc)
    {
    case SORT_NONE:return QString();
    case SORT_FILENAME:return QObject::tr("Filename");
    case SORT_FULLNAME:return QObject::tr("Fullname");
    case SORT_SIZE:return QObject::tr("Size");
    case SORT_WTIME:return QObject::tr("Wtime");
    case SORT_RESOLUTION: return QObject::tr("Resolution");
    case SORT_DURATION:return QObject::tr("Duration");
    case SORT_BITRATE:return QObject::tr("Bitrate");
    case SORT_OPENCOUNT:return QObject::tr("Open count");
    case SORT_LASTACCESS:return QObject::tr("Last Access");
    case SORT_FPS:return QObject::tr("FPS");
    default:
        Q_ASSERT(false);

    }
    return QString();
}

QString GetAppDir()
{
    return QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
}


void InsertUniqueTextToComboBox(QComboBox& combo, const QString& text)
{
    if(text.isEmpty())
        return;

    QList<int> dels;
    for(int i=0 ; i < combo.count(); ++i)
    {
        QString t = combo.itemText(i);
        if(t==text)
            dels.append(i);
    }

    QList<int>::const_reverse_iterator rit = dels.crbegin();
    for (; rit != dels.crend(); ++rit)
    {
        int deli = *rit;
        combo.removeItem(deli);
    }

    combo.insertItem(0, text);
}



bool processCommandLine(QString* helpText)
{
    QCommandLineParser parser;
    // parser.setApplicationDescription("SceneExplorer");

    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();

    parser.addPositionalArgument("document", QCoreApplication::translate("main", "document to open."));

    //    parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));

    // A boolean option with a single name (-p)
    //    QCommandLineOption showProgressOption("p", QCoreApplication::translate("main", "Show progress during copy"));
    //    parser.addOption(showProgressOption);

    // A boolean option with multiple names (-f, --force)
    //    QCommandLineOption forceOption(QStringList() << "f" << "force",
    //                                   QCoreApplication::translate("main", "Overwrite existing files."));
    //    parser.addOption(forceOption);

    // An option with a value
    QCommandLineOption dbDirectoryOption(QStringList() << "d" << "database-directory",
                                         QCoreApplication::translate("main", "Set database directory <directory>."),
                                         "directory");
    parser.addOption(dbDirectoryOption);

    // A boolean option with a single name (-n)
    QCommandLineOption notAddToRecent("n",
                                      QCoreApplication::translate("main", "Do not add to recent open documents."));
    parser.addOption(notAddToRecent);



    // Process the actual command line arguments given by the user
    parser.parse(QCoreApplication::arguments());

    if(helpText != nullptr)
    {
        *helpText = parser.helpText();
        return false;
    }
    if(parser.isSet(helpOption))
    {
        // Info(nullptr, parser.sh);
        parser.showHelp();
        return false;
    }
    if(parser.isSet(versionOption))
    {
        parser.showVersion();
        return false;
    }

    QString dbdir = parser.value(dbDirectoryOption);
    QString doc;

    const QStringList args = parser.positionalArguments();
    if(!args.isEmpty())
    {
        doc = parser.value(QString());
        doc = args[0];
        if(doc=="/?")
        {
            parser.showHelp();
            return false;
        }
    }

    Q_ASSERT(!gpCommandOption.get());
    gpCommandOption.reset(new CommandOption(dbdir, doc, parser.isSet(notAddToRecent)));

    return true;
}

QString ExpandEnv(const QString& str)
{
    static QRegExp rx("(\\$\\{\\w+\\})");
    QString result;
    int prevpos = 0;
    int pos = 0;

    while ((pos = rx.indexIn(str, pos)) != -1)
    {
        result += str.mid(prevpos,pos-prevpos);
        int matchedlen = rx.matchedLength();
        QString s = str.mid(pos,matchedlen);
        if(s==STR_ENV_SCENEEXPLORER_ROOT)
        {
            static QString thisdir = GetAppDir();
            result += thisdir;
        }
        else
        {
            result += str.mid(pos, matchedlen);
        }
        pos += matchedlen;
        prevpos = pos;
    }
    result += str.mid(prevpos);

    return result;
}


// In linux, constructor with sql string cause sigxxx, so this is temporal fix
QSqlQuery myPrepare(const QString& sql)
{
    QSqlQuery q;
    if(!q.prepare(sql))
    {
        qDebug() << q.lastError() << __FUNCTION__;
        Q_ASSERT(false);
        return q;
    }
    return q;
}

QString getClipboardText()
{
    return QApplication::clipboard()->text();
}
void setClipboardText(const QString& text)
{
    QApplication::clipboard()->setText(text);
}
bool IsClipboardTagDataAvalable()
{
    QString clipText = getClipboardText();
    QStringList lines = clipText.split('\n');
    if (lines.isEmpty())
        return false;

    if (lines[0].startsWith(STR_TAG_ENTRY_SIGNATURE))
        return true;

    return false;
}

bool isLegalFileExt(QString ext)
{
    if(ext.isEmpty())
        return false;

    if(ext[0]=='.')
    {
        ext = ext.right(ext.length()-1);
        return isLegalFileExt(ext);
    }

    if(ext.contains('/') || ext.contains('\\'))
        return false;

    return isLegalFilePath(ext, nullptr);
}
bool isThumbFileName(const QString& file)
{
    // ex) 58c4d22e-8b8b-4773-9fac-80a69a8fa880-5.jpg
    static QRegExp rx(
                "^"
                "[a-fA-F0-9]{8}"
                "-"
                "[a-fA-F0-9]{4}"
                "-"
                "[a-fA-F0-9]{4}"
                "-"
                "[a-fA-F0-9]{4}"
                "-"
                "[a-fA-F0-9]{12}"
                "-"
                "[0-9]+"
                "\\."
                "[a-zA-Z0-9]+");

    Q_ASSERT(rx.isValid());
    return rx.exactMatch(QFileInfo(file).fileName());
}
bool isUUID(const QString& s)
{
    // ex) B4149711-6824-4606-B601-AC89C1BEC092
    static QRegExp rx(
                "^"
                "[a-fA-F0-9]{8}"
                "-"
                "[a-fA-F0-9]{4}"
                "-"
                "[a-fA-F0-9]{4}"
                "-"
                "[a-fA-F0-9]{4}"
                "-"
                "[a-fA-F0-9]{12}"
                "$");
    Q_ASSERT(rx.isValid());
    return rx.exactMatch(s);
}

//std::wstring qToStdWString(const QString &str)
//{
//#ifdef _MSC_VER
//    return std::wstring((const wchar_t*)str.utf16());
//#else
//    return str.toStdWString();
//#endif
//}
//QString stdWToQString(const std::wstring &str)
//{
//#ifdef _MSC_VER
//    return QString::fromUtf16((const ushort*)str.c_str());
//#else
//    return QString::fromStdWString(str);
//#endif
//}

bool IsSubDir(const QString& parent, const QString& child)
{
    QDir childDir(child);
    do
    {
        if(QDir(parent)==childDir)
            return true;
    } while(childDir.cdUp());

    return false;
}

QStringList RemoveDuplicateSubDirectory(const QStringList& sources, QStringList& removedRet)
{
    QStringList canonicalSources;
    for(auto&& s : sources)
        canonicalSources.append(QFileInfo(s).canonicalFilePath());

    QStringList canonicalResults{canonicalSources};
    QStringList results{sources};

    foreach(auto&& firstLoopElem, canonicalSources)
    {
        foreach(auto&& secondLoopElem, canonicalResults)
        {
            // IsSubDir returns true for same directory
            if(firstLoopElem != secondLoopElem && IsSubDir(firstLoopElem, secondLoopElem))
            {
                // Remove testee.second from results
                bool done = false;
                do
                {
                    // Use lambda for break by return
                    [&]()
                    {
                        // Remove secondLoopElem from results
                        for(auto&& s : results)
                        {
                            QString c = QFileInfo(s).canonicalFilePath();
                            if(c == secondLoopElem)
                            {
                                qDebug() << "removed " << s << "(" << c << ")" << __FUNCTION__;
                                removedRet.append(s);

                                qDebug() << "results " << results << __FUNCTION__;
                                results.removeOne(s);
                                qDebug() << "results " << results << __FUNCTION__;
                                return;
                            }
                        }
                        done = true;
                    }();
                }while(!done);
            }
        }
    }

    return results;
}


// http://comments.gmane.org/gmane.comp.lib.qt.general/34914
QString size_human(const qint64& size)
{
    float num = size;
    static const QStringList list{ "KiB", "MiB", "GiB", "TiB"};

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= FLOAT1024 && i.hasNext())
    {
        unit = i.next();
        num /= FLOAT1024;
    }
    return QString().setNum(num,'f',2)+ unit;
}

QString filetime_human(const qint64& filetime)
{
    QDateTime qdt = QDateTime::fromTime_t(filetime);
    return doublequoteIfNecessary(qdt.toString(Qt::DateFormat::DefaultLocaleShortDate));
}
QString fps_human(const double& fps)
{
    return QString::number(fps,'g',4);
}
QString duration_human(double duration)
{
    int iduration = (int)(duration);

    int hour = iduration/3600;
    int minutes = (iduration%3600)/60;
    int seconds = iduration%60;

    return
            QString("%1").arg((int)hour, 2, 10, QChar('0')) + ":"+
            QString("%1").arg((int)minutes, 2, 10, QChar('0'))+":"+
            QString("%1").arg((int)seconds, 2, 10, QChar('0'));

}
QString format_human(const QString& s)
{
    return s.split(',')[0];
}
QString bitrate_human(int bitrate)
{
    float num = bitrate;
    static const QStringList list{"kb/s","mb/s","gb/s","tb/s"};

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= FLOAT1000 && i.hasNext())
    {
        unit = i.next();
        num /= FLOAT1000;
    }
    return QString().setNum(num,'f',2)+ unit;
}
QString resolution_human(int width, int height)
{
    return QString::number(width) + "x" + QString::number(height);
}
QString opencount_human(int count)
{
    return QString::number(count);
}
