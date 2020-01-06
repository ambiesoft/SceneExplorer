#include <QCoreApplication>
#include <QStorageInfo>
#include <QDebug>
#include <QStringList>

#include "../../../../lsMisc/stdQt/stdQt.h"
using namespace AmbiesoftQt;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStorageInfo storage = QStorageInfo::root();

    qDebug() << storage.rootPath();
    if (storage.isReadOnly())
        qDebug() << "isReadOnly:" << storage.isReadOnly();

    qDebug() << "name:" << storage.name();
    qDebug() << "fileSystemType:" << storage.fileSystemType();
    qDebug() << "size:" << storage.bytesTotal()/1000/1000 << "MB";
    qDebug() << "availableSize:" << storage.bytesAvailable()/1000/1000 << "MB";


    // https://www.ics.com/blog/whats-new-qt-54-qstorageinfo-class
    for (auto volume : QStorageInfo::mountedVolumes()) {
         qDebug() << "Name:" << volume.name();

         // "\\\\?\\Volume{ba447492-0000-0000-0040-000000000000}\\"
         // "\\\\Thexp\\EPWING\\"
         qDebug() << "Display name:" << volume.displayName();
         qDebug() << "Device:" << volume.device();
         qDebug() << "Root path:" << volume.rootPath();
         qDebug() << "File system type:" << volume.fileSystemType();
         qDebug() << "Is valid?" << (volume.isValid() ? "yes" : "no");
         qDebug() << "Is root?" << (volume.isRoot() ? "yes" : "no");
         qDebug() << "Is ready?" << (volume.isReady() ? "yes" : "no");
         qDebug() << "Is read only?" << (volume.isReadOnly() ? "yes" : "no");
         qDebug() << "Bytes available:" << volume.bytesAvailable();
         qDebug() << "Bytes free:" << volume.bytesFree();
         qDebug() << "Bytes total:" << volume.bytesTotal() << endl;
    }

    QStringList dirs;
    dirs << "C:\\T" << "Y:\\Share" << "Z:\\zShare" << "\\\\Thexp\\EPWING"
         << "C:\\code\\cg" << "G:\\vcpkg" << "Y:\\ProgShare\\FFmpegVideoCut-master"
         << "Z:\\My Applications";

    QStringList dirsResult = SortDevice1by1(dirs);

    Q_ASSERT(IsSameContents(dirs,dirsResult));
    return a.exec();
}
