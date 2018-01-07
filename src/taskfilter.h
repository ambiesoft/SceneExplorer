#ifndef TASKFILTER_H
#define TASKFILTER_H

#include <QRunnable>
#include <QObject>
#include <QVector>
class TaskFilter : public QObject, public QRunnable
{
    Q_OBJECT
private:
    int id_;
    QString dir_;
    QStringList filesin_;
    QStringList entries_;
    QVector<qint64> sizes_;
    QVector<qint64> ctimes_;
    QVector<qint64> wtimes_;
    QStringList salients_;

    void runStuff(const QString& dir);

public:
    TaskFilter(int id,
               const QString& dir,
               const QStringList& filesin,
               const QStringList& entries,
               const QVector<qint64>& sizes,
               const QVector<qint64>& ctimes,
               const QVector<qint64>& wtimes,
               const QStringList& salients):
        id_(id),
        dir_(dir),
        filesin_(filesin),
        entries_(entries),
        sizes_(sizes),
        ctimes_(ctimes),
        wtimes_(wtimes),
        salients_(salients)
    {}

    void run() override;

signals:
    void afterFilter(int id,
                     const QString& dir,
                     const QStringList& files);
};

#endif // TASKFILTER_H
