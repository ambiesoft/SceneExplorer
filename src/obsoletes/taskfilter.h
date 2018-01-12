#ifndef TASKFILTER_H
#define TASKFILTER_H


class TaskFilter : public QObject, public QRunnable
{
    Q_OBJECT
private:
    int loopId_;
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
    TaskFilter(int loopId, int id,
               const QString& dir,
               const QStringList& filesin,
               const QStringList& entries,
               const QVector<qint64>& sizes,
               const QVector<qint64>& ctimes,
               const QVector<qint64>& wtimes,
               const QStringList& salients):
        loopId_(loopId),id_(id),
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
    void afterFilter(int loopId, int id,
                     const QString& dir,
                     const QStringList& files,
                     const QStringList& renameOlds,
                     const QStringList& renameNews);
    void finished_Filter(int loopId, int id);
};

#endif // TASKFILTER_H
