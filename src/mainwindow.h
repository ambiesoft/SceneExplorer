#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QFileIconProvider>

#include "tablemodel.h"
#include "directoryentry.h"
#include "directoryitem.h"

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

    bool IsInitialized() const;

private:
    enum IDKIND {
        IDKIND_GetDir,
        // IDKIND_Filter,
        IDKIND_FFmpeg,
    };
	bool initialized_ = false;
	bool closed_ = false;
	bool directoryChanging_ = false;
    QStringList currentDirs_;
    // bool bShowMissing_ = false;
    void GetSqlAllSetTable(const QStringList& dirs);
	QToolButton* btnShowNonExistant_ = nullptr;
    class IDManager
    {
        MainWindow* win_;

        int idGetDir_ = 0;
        // int idFilter_ = 0;
        int idFFMpeg_ = 0;

        int idGetDirDone_ = 0;
        // int idFilterDone_ = 0;
        int idFFMpegDone_ = 0;

        void updateStatus();
    public:
        IDManager(MainWindow* win):win_(win){}

        int Get(IDKIND idkind) const
        {
            switch(idkind)
            {
            case GetDir: return idGetDir_;
            // case Filter: return idFilter_;
            case FFMpeg: return idFFMpeg_;
            }
            Q_ASSERT(false);
            return 0;
        }
        int GetDone(IDKIND idkind) const
        {
            switch(idkind)
            {
            case GetDir: return idGetDirDone_;
            // case Filter: return idFilterDone_;
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
            // case Filter: ret = ++idFilter_;break;
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
            // case Filter: ret = ++idFilterDone_;break;
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
            // idFilter_ = idFilterDone_ = 0;
            idFFMpeg_ = idFFMpegDone_=0;
            updateStatus();
        }
        bool isAllTaskFinished() const
        {
            if(
                    Get(IDKIND_FFmpeg)==GetDone(IDKIND_FFmpeg) &&
                    Get(IDKIND_GetDir)==GetDone(IDKIND_GetDir)
              )
            {
                return true;
            }
            return false;
        }
    }* idManager_ = nullptr;
    friend class IDManager;

    void checkTaskFinished();

    int threadcountGetDir_ = 1;
    int threadcountFFmpeg_ = 2;
    // int threadcountFilter_ = 1;

    bool initShown=false;
	bool IsClosed() const {
		return closed_;
	}

    QLabel *slPaused_ = nullptr;
    QLabel *slTask_ = nullptr;
    QLabel *slItemCount_ = nullptr;
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

    void on_directoryWidget_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void on_action_Top_triggered();

    void on_action_Bottom_triggered();

    void on_directoryWidget_itemChanged(QListWidgetItem *item);

    void on_directoryWidget_customContextMenuRequested(const QPoint &pos);

    void on_actionSort_by_resolution_triggered();

    void on_actionSort_by_duration_triggered();

    void on_actionSort_by_bitrate_triggered();
    void on_action_ShowMissing(bool bToggle);
private:
    QThreadPool* pPoolFFmpeg_ = nullptr;
    QThreadPool* getPoolFFmpeg();

    QThreadPool* pPoolGetDir_ = nullptr;
    QThreadPool* getPoolGetDir();

//    QThreadPool* pPoolFilter_ = nullptr;
//    QThreadPool* getPoolFilter();

    void clearAllPool();

    void AddUserEntryDirectory(
            DirectoryItem::DirectoryItemType itemType,
            const QString& dir,
            bool sel,
            bool check);
    void directoryChangedCommon(bool bForceRead=false);
    void StartScan(const QString& dir);
    void StartScan(QListWidgetItem* item);
    void StartScan2(const QString& dir);

    QFileIconProvider fiProvider_;

    Ui::MainWindow *ui;
    TableModel* tableModel_;
	FileMissingFilterProxyModel *proxyModel_;
    QString lastSelectedDir_;

    TaskModel* taskModel_;

    void resizeDock(QDockWidget* dock, const QSize& size);

    enum TaskKind {
        GetDir,
        // Filter,
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
                   int bitrate,
                   const QString& vcodec,
                   const QString& acodec,
                   int vWidth,int vHeight);
    void sayDead(int loopId, int id);
    void finished_FFMpeg(int loopId, int id);

    void afterGetDir(int loopId, int id,
                     const QString& dir,
                     const QStringList& files,
		const QStringList& salients);
    void finished_GetDir(int loopId, int id ,const QString& dir);
//    void afterFilter(int loopId, int id,
//                     const QString& dir,
//                     const QStringList& filteredFiles,
//                     const QStringList& renameOlds,
//                     const QStringList& renameNews);
    void afterFilter2(int loopId, int id,
                     const QString& dir,
                     const QStringList& filteredFiles);
//    void finished_Filter(int loopId, int id);

    void openSelectedVideo();
    void openSelectedVideoInFolder();
    void copySelectedVideoPath();
    void copySelectedVideoFilename();

    void tableItemCountChanged();

	void on_Rescan();
	void on_directoryWidget_Remove();
	void on_directoryWidget_UncheckAll();
};

#endif // MAINWINDOW_H
