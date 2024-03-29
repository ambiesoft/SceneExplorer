#ifndef TASKCHECKTHUMBS_H
#define TASKCHECKTHUMBS_H

#include <QObject>
#include <QRunnable>

#include "IFFTask2Main.h"

class TaskCheckThumbs : public QObject, public QRunnable
{
    Q_OBJECT
    int loopid_;
    int taskindex_;
    int getdirid_;
    QString dir_;
    QString file_;
    QString thumbid_;
    QString thumbext_;
    int width_=0;
    int height_=0;

    const IFFTask2Main* pFF2M_ = nullptr;
public:
    explicit TaskCheckThumbs(int loopid,
                             int taskindex,
                             int getdirid,
                             const QString& dir,
                             const QString& file,
                             const QString& thumbid,
                             const QString& thumbext,
                             int width, int height,
                             const IFFTask2Main* pFF2M) :
        loopid_(loopid),
        taskindex_(taskindex),
        getdirid_(getdirid),
        dir_(dir),
        file_(file),
        thumbid_(thumbid),
        thumbext_(thumbext),
        width_(width), height_(height),
        pFF2M_(pFF2M) {}
    virtual ~TaskCheckThumbs() override {}

    void run() override;

Q_SIGNALS:
    void afterCheckThumbs(int loopId,
                          int taskindex,
                          int getdirid,
                          const QString& dir,
                          const QString& file,
                          const QString& thumbid,
                          int width,
                          int height,
                          bool bExists);
    void finished_CheckThumbs(int loopId, int id);
};

#endif // TASKCHECKTHUMBS_H
