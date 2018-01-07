#ifndef LISTITEMDATA_H
#define LISTITEMDATA_H

#include <QStringList>

class TableItemData
{
    QStringList files_;
    int width_;
    int height_;
    QString movieFile_;
    QString format_;
    mutable qint64 size_=-1;
public:
    TableItemData(const QStringList& files,
             int width, int height,
             const QString& movieFile,
             const QString& format);

    QStringList getImageFiles() const {
        return files_;
    }
    int getWidth() const {
        return width_;
    }
    int getHeight() const {
        return height_;
    }
    QString getMovieFile() const {
        return movieFile_;
    }
    QString getFormat() const {
        return format_;
    }
    qint64 getSize() const;


};

#endif // LISTITEMDATA_H
