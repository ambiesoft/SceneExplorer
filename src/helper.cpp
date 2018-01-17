#include <QDir>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QFileInfo>
#include <QDesktopServices>
#include <QSet>

#if defined(Q_OS_WIN)
#else

#endif

#include "consts.h"
#include "helper.h"

QString rstrip(const QString& str, QChar c) {
  int n = str.size() - 1;
  for (; n >= 0; --n) {
    if (str.at(n) != c) {
      return str.left(n + 1);
    }
  }
  return "";
}

QString pathCombine(const QString& path1, const QString& path2)
{
    if(QDir::isAbsolutePath(path2))
        return path2;


    return rstrip(path1, '/') + '/' + rstrip(path2, '/');
}
QString canonicalDir(const QString& dir)
{
    if(!dir.endsWith('/'))
        return dir+'/';
    return rstrip(dir, '/') + '/';
}
void Info(QWidget* parent, QString message)
{
	QMessageBox msgBox;
	msgBox.setParent(parent);
	msgBox.setWindowTitle(Consts::APPNAME);
	// msgBox.setInformativeText(message);
	msgBox.setText(message);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setIcon(QMessageBox::Information);
	msgBox.exec();
}
void Alert(QWidget* parent, QString message)
{
	QMessageBox msgBox;
	msgBox.setParent(parent);
	// msgBox.setInformativeText(message);
	msgBox.setText(message);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}
bool YesNo(QWidget* parent, QString message)
{
	QMessageBox msgBox;
	msgBox.setParent(parent);
	msgBox.setWindowTitle(Consts::APPNAME);
	msgBox.setText(message);
	msgBox.setStandardButtons(QMessageBox::Yes);
	msgBox.addButton(QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	msgBox.setIcon(QMessageBox::Question);
	return msgBox.exec() == QMessageBox::Yes;
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

bool IsVideoExtention(const QString& file)
{
    // static const char* sexts[] = {
    static QSet<QString> sqExts = {
        "3g2",
        "3g2",
        "3gp",
        "3gp",
        "amv",
        "asf",
        "asf",
        "avi",
        "avi",
        "avs",
        "divx",
        "drc",
        "f4a",
        "f4b",
        "f4p",
        "f4v",
        "flv",
        "flv",
        "flv",
        "flv",
        "gif",
        "gifv",
        "m2v",
        "m4p",
        "m4v",
        "m4v",
        "m4v",
        "mkv",
        "mkv",
        "mng",
        "mov",
        "mov",
        "mp2",
        "mp4",
        "mp4",
        "mpe",
        "mpeg",
        "mpeg",
        "mpeg",
        "mpg",
        "mpg",
        "mpg",
        "mpv",
        "mxf",
        "nsv",
        "ogg",
        "ogm",
        "ogv",
        "qt",
        "rm",
        "rm",
        "rmvb",
        "rmvb",
        "roq",
        "svi",
        "swf",
        "vob",
        "webm",
        "wmv",
        "wmv",
        "yuv",
    };


    int li = file.lastIndexOf('.');
    if (li < 0)
        return false;

    QString ext = file.right(file.length() - li - 1);
    ext = ext.toLower();
    return sqExts.contains(ext);
}
