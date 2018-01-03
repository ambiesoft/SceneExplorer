#ifndef LISTITEMDATA_H
#define LISTITEMDATA_H

#include <QStringList>

class ListItemData
{
    QStringList files_;
    int width_;
    int height_;
    QString movieFile_;
public:
    ListItemData(const QStringList& files, int width, int height, const QString& movieFile);

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

};

#endif // LISTITEMDATA_H
