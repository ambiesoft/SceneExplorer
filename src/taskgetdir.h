#ifndef TASKGETDIR_H
#define TASKGETDIR_H



class TaskGetDir : public QObject, public QRunnable
{
    Q_OBJECT
private:
    int loopId_;
    int id_;

    QString dir_;

    void runStuff(const QString& dir);

public:
    TaskGetDir(int loopId, int id, const QString& dir):
        loopId_(loopId),
        id_(id),
        dir_(dir){}

    void run() override;

signals:
    void afterGetDir(int loopId, int id,
                     const QString& dir,
                     const QStringList& files,
		const QStringList& salients);
    void finished_GetDir(int loopId,int id,const QString& dir);
};

#endif // TASKGETDIR_H
