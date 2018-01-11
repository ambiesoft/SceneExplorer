#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tablemodel.h"

class QThreadPool;
class QLabel;
class ListModel;
class TableItemData;
class FolderModel;
class Settings;
class TaskModel;
class TaskFFmpeg;
class QLabel;
class QItemSelection;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    // setting keys


public:
    explicit MainWindow(QWidget *parent, Settings& settings);
    ~MainWindow();

private:
    enum IDKIND {
        IDKIND_GetDir,
        IDKIND_Filter,
        IDKIND_FFmpeg,
    };
    class IDManager
    {
        MainWindow* win_;

        int idGetDir_ = 0;
        int idFilter_ = 0;
        int idFFMpeg_ = 0;

        int idGetDirDone_ = 0;
        int idFilterDone_ = 0;
        int idFFMpegDone_ = 0;

        void updateStatus();
    public:
        IDManager(MainWindow* win):win_(win){}

        int Get(IDKIND idkind)
        {
            switch(idkind)
            {
            case GetDir: return idGetDir_;
            case Filter: return idFilter_;
            case FFMpeg: return idFFMpeg_;
            }
            Q_ASSERT(false);
            return 0;
        }
        int GetDone(IDKIND idkind)
        {
            switch(idkind)
            {
            case GetDir: return idGetDirDone_;
            case Filter: return idFilterDone_;
            case FFMpeg: return idFFMpegDone_;
            }
            Q_ASSERT(false);
            return 0;
        }
        int Increment(IDKIND idkind)
        {
            int ret;
            switch(idkind)
            {
            case GetDir: ret = ++idGetDir_;break;
            case Filter: ret = ++idFilter_;break;
            case FFMpeg: ret = ++idFFMpeg_;break;
            default:
            Q_ASSERT(false);
            }
            updateStatus();
            return ret;
        }
        int IncrementDone(IDKIND idkind)
        {
            int ret;
            switch(idkind)
            {
            case GetDir: ret = ++idGetDirDone_;break;
            case Filter: ret = ++idFilterDone_;break;
            case FFMpeg: ret = ++idFFMpegDone_;break;
            default:
            Q_ASSERT(false);
            }
            updateStatus();
            return ret;
        }
        void Clear()
        {
            idGetDir_ = idGetDirDone_ = 0;
            idFilter_ = idFilterDone_ = 0;
            idFFMpeg_ = idFFMpegDone_=0;
            updateStatus();
        }
    }* idManager_ = nullptr;
    friend class IDManager;

    int threadcountGetDir_ = 1;
    int threadcountFFmpeg_ = 2;
    int threadcountFilter_ = 1;

    bool initShown=false;

    QLabel *slPaused_ = nullptr;
    QLabel *slTask_ = nullptr;

protected:
    virtual void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void showEvent( QShowEvent* event );

private slots:
    void on_action_Close_triggered();
    void on_action_Do_It_triggered();
    void on_action_About_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_action_Options_triggered();
    void on_action_Pause_triggered();
    void on_action_Stop_triggered();
    void onMenuTask_AboutToShow();
    void onMenuDocking_windows_AboutToShow();

    void on_actionSort_by_file_name_triggered();
    void on_actionSort_by_file_size_triggered();
    void on_actionSort_by_wtime_triggered();

    void on_tableView_customContextMenuRequested(const QPoint &pos);

    void on_treeView_activated(const QModelIndex &index);

    void on_treeView_clicked(const QModelIndex &index);
    void on_treeView_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QThreadPool* pPoolFFmpeg_ = nullptr;
    QThreadPool* getPoolFFmpeg();

    QThreadPool* pPoolGetDir_ = nullptr;
    QThreadPool* getPoolGetDir();

    QThreadPool* pPoolFilter_ = nullptr;
    QThreadPool* getPoolFilter();

    void clearAllPool();

    Ui::MainWindow *ui;
    TableModel* tableModel_;
    QString lastSelectedDir_;

    FolderModel* treeModel_;

    TaskModel* taskModel_;

    void resizeDock(QDockWidget* dock, const QSize& size);

    enum TaskKind {
        GetDir,
        Filter,
        FFMpeg,
        SQL,
        App,
    };
    void insertLog(TaskKind kind, int id, const QString& text, bool bError=false);
    void insertLog(TaskKind kind, const QVector<int>& ids, const QStringList& texts, bool bError=false);

    void openVideo(const QString& movieFile);
    void openVideoInFolder(const QString& movieFile);
    QString getSelectedVideo();
public slots:
//    void sayBorn(int id,
//                   const QString& movieFile);
    void sayHello(int loopId, int id,
                   const QString& movieFile);
    void sayNo(int loopId, int id,
                   const QString& movieFile,
                   const QString& errorReason);
    void sayGoodby(int loopId, int id,
                   const QStringList& files,
                   const QString& movieFile,
                   int thumbwidth,
                   int thumbheight,
                   const double& duration,
                   const QString& format,
                   const QString& vcodec,
                   const QString& acodec,
                   int vWidth,int vHeight);
    void sayDead(int loopId, int id);
    void finished_FFMpeg(int loopId, int id);

    void afterGetDir(int loopId, int id,
                     const QString& dir,
                     const QStringList& files);
    void finished_GetDir(int loopId, int id);
    void afterFilter(int loopId, int id,
                     const QString& dir,
                     const QStringList& filteredFiles,
                     const QStringList& renameOlds,
                     const QStringList& renameNews);
    void finished_Filter(int loopId, int id);

    void openSelectedVideo();
    void openSelectedVideoInFolder();
    void copySelectedVideoPath();
};

#endif // MAINWINDOW_H
