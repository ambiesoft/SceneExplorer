//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QFileIconProvider>
#include <QPushButton>

#include "tablemodel.h"
#include "directoryentry.h"
#include "directoryitem.h"
#include "findcombobox.h"
#include "settings.h"
#include "externaltoolitem.h"
#include "document.h"
#include "blockedbool.h"
#include "imainwindow.h"

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

typedef QSharedPointer<ExternalToolItem> ExternalToolItemPointer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, IMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;
    // setting keys
    Settings& settings_;
    Document* pDoc_ = nullptr;
    QStringList recents_;

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
    //SORTCOLUMN currentSort_ = SORTCOLUMN::SORT_NONE;
    //bool currentSortRev_ = false;

    // bool bShowMissing_ = false;
    void GetSqlAllSetTable(const QStringList& dirs,
                           bool bOnlyMissing = false);
    QToolButton* tbShowNonExistant_ = nullptr;

    enum UpdateTitleType { DEFAULT, ONLYMISSING, INIT };
    void UpdateTitle(const QStringList& dirs, UpdateTitleType utt);

    QList<ExternalToolItem> externalTools_;

    class LimitManager
    {
        // int curIndex_ = 0;
        int numOfRows_;
		qlonglong allCount_ = -1;
        QComboBox* cmb_;
        bool blocking_ = false;
    public:
        LimitManager(int numOfRows, QComboBox* cmb) : numOfRows_(numOfRows), cmb_(cmb){}
        void Reset() {
            BlockedBool bb(&blocking_);
            cmb_->setCurrentIndex(0);
			allCount_ = -1;
        }
        bool Decrement();
        bool Increment();
        int GetCurrentIndex() const {
            return cmb_->currentIndex();
        }

        int GetNumberOfRows() const {
            return numOfRows_;
        }
        void SetNumberOfRows(int i) {
            numOfRows_ = i;
        }
		bool IsNotCounted() const {
			return allCount_ == -1;
		}
		qlonglong GetAllCount() const {
			return allCount_;
		}
		void SetAllCount(qlonglong& l) {
			allCount_ = l;
		}
        void SetIndexFirst() {
            BlockedBool bb(&blocking_);
            cmb_->setCurrentIndex(0);
        }
        void SetIndexLast() {
            BlockedBool bb(&blocking_);
            cmb_->setCurrentIndex(cmb_->count()-1);
        }
        bool* GetBlockPointer() {
            return &blocking_;
        }
        bool IsBlocking() const {
            return blocking_;
        }
    };
    LimitManager* limitManager_ = nullptr;

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

    int optionThreadcountGetDir_ = 1;
    int optionThreadcountThumbnail_ = 2;
    int optionThumbCount_ = 3; // default is 3

    bool initShown=false;
	bool IsClosed() const {
		return closed_;
	}

    QLabel *slPaused_ = nullptr;
    QLabel *slTask_ = nullptr;
    QLabel *slItemCount_ = nullptr;
    QLabel *slItemSort_ = nullptr;

    FindComboBox* cmbFind_ = nullptr;

    QAction* actionLimitFirst_ = nullptr;
    QAction* actionLimitPrev_ = nullptr;
    QComboBox* cmbLimit_ = nullptr;
    QAction* actionLimitNext_ = nullptr;
    QAction* actionLimitLast_ = nullptr;
    QAction* sepLimit_ = nullptr;

    void onTaskStarted();
    void onTaskEnded();
    // QTimer* taskMonitorTimer = nullptr;


protected:
    virtual void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void showEvent( QShowEvent* event );

    void initLangMenus();
	class SortManager
	{
		SORTCOLUMNMY sort_;
        bool ascending_[COUNT_SORTCOLUMN];
        QIcon iconsAscend_[COUNT_SORTCOLUMN];
        QIcon iconsDescend_[COUNT_SORTCOLUMN];
		QToolButton* tbs_[COUNT_SORTCOLUMN];
	public:
		SortManager();
		void onSort(SORTCOLUMNMY sc);
		void UpdateButtonText();
		SORTCOLUMNMY GetCurrentSort() const {
			return sort_;
		}
		bool GetCurrentRev() const {
            return ascending_[sort_];
		}
        void setToolButton(SORTCOLUMNMY sc,
                           QToolButton* tb,
                           const QIcon& iconNormal,
                           const QIcon& iconRev);
//        void setToolButton(SORTCOLUMN sc,
//                           QToolButton* tb) {
//            setToolButton(sc,tb,QIcon(),QIcon());
//        }

		void InitCurrentSort(SORTCOLUMNMY sc, bool b) {
			sort_ = sc;
            ascending_[sort_] = b;
			UpdateButtonText();
		}
	} sortManager_;
	void onSortCommon(SORTCOLUMNMY sortColumn);
	QToolButton* tbNameSort_ = nullptr;
    QToolButton* tbSizeSort_ = nullptr;
    QToolButton* tbWtime_ = nullptr;
    QToolButton* tbOpenCount_ = nullptr;
    QToolButton* tbLastAccess_ = nullptr;

    virtual SORTCOLUMNMY GetCurrentSort() {
        return sortManager_.GetCurrentSort();
    }
	QToolButton* tbLabelSort_ = nullptr;
	
    void on_ShowMissingClicked_common(bool nNextCheck);

private slots:
    void on_action_Close_triggered();
    void on_action_About_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_action_Options_triggered();
    void on_action_Pause_triggered();
    void on_action_Stop_triggered();
    void onMenuTask_AboutToShow();
    void onMenuLanguage_AboutToShow();
    void onMenuDocking_windows_AboutToShow();
    // void onMenu_Favorites_AboutToShow();
    void onMenu_RecentDocuments_AboutToShow();

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
    void on_action_ShowMissingClicked();
    void on_action_Find_triggered();

    void on_action_Clear_triggered();

//    void on_action_Add_current_check_states_triggered();
//    void on_FavoriteFolder_triggered(bool checked=false);

    void on_RecentDocuments_triggered(bool checked=false);

    // void onTaskTimerTick();
    void on_actionSort_by_open_count_triggered();

    void on_action_Add_Folder_triggered();

    void on_action_Extentions_triggered();

    void on_action_Font_triggered();

    void on_action_FontDetail_triggered();

    void on_action_Output_triggered();

    void on_actionExternal_Tools_triggered();

    void on_action_Open_triggered();

    void on_action_Save_triggered();

    void on_actionSave_as_triggered();

    void on_actionAbout_document_triggered();

    void on_action_New_triggered();


    void on_LimitFirst_triggered(bool checked=false);
    void on_LimitPrev_triggered(bool checked=false);
    void on_LimitNext_triggered(bool checked=false);
    void on_LimitLast_triggered(bool checked=false);
    void onCmbLint_currentIndexChanged(int index);
    void on_actionStart_scan_to_create_thumnails_triggered();

    void on_action_Focus_find_triggered();

    void on_action_Empty_find_texts_triggered();

    void on_actionSort_by_last_access_triggered();




    void on_actionShow_missing_files_triggered();

    void on_action_DockFolder_triggered();

    void on_action_DockTask_triggered();

    void on_action_DockOutput_triggered();



    void on_action_English_triggered();

    void on_action_Japanese_triggered();

    void on_action_System_default_triggered();

    void on_action_Command_Line_triggered();

private:
    QThreadPool* pPoolFFmpeg_ = nullptr;
    QThreadPool* getPoolFFmpeg();

    QThreadPool* pPoolGetDir_ = nullptr;
    QThreadPool* getPoolGetDir();

//    QThreadPool* pPoolFilter_ = nullptr;
//    QThreadPool* getPoolFilter();

    void clearAllPool(bool bAppendLog=true);

    void AddUserEntryDirectory(
            DirectoryItem::DirectoryItemType itemType,
            const QString& dir,
            bool sel,
            bool check);
    void directoryChangedCommon(bool bForceRead=false);

    void langChanged_common(const QString& lang);

    void StartScan(QListWidgetItem* item);
    void StartScan(const QString& dir);

    QFileIconProvider fiProvider_;


    TableModel* tableModel_;
	FileMissingFilterProxyModel *proxyModel_;
    QString lastSelectedAddFolderDir_;
    QString lastSelectedScanDir_;

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
    QString getSelectedVideo(bool bNativeFormat = true);
    bool IsDirSelected(const QString& dir) const;

    bool checkExeCommon(const QString& exe, QString& errString) const;
    bool checkFFprobe(QString& errString) const;
    bool checkFFmpeg(QString& errString) const;

    void CreateLimitManager();
    QString GetDefaultDocumentPath();
public:
    bool OpenDocument(const QString& file, const bool bExists);
    void InitDocument();

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

                     const QList<qint64> sizes,
                     const QList<qint64> wtimes,

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

    void on_context_openSelectedVideo();
    void on_context_openSelectedVideoInFolder();
    void on_context_copySelectedVideoPath();
    void on_context_copySelectedVideoFilename();
    void on_context_removeFromDatabase();
    void on_context_Rename();
    void on_context_ExternalTools();

    void tableItemCountChanged();
    void tableSortParameterChanged(SORTCOLUMNMY sc, bool rev);

	void on_Rescan();
	void on_directoryWidget_Remove();
	void on_directoryWidget_RemoveMissingItems();
	void on_directoryWidget_UncheckAll();
	void on_directoryWidget_SortByName();
	void on_directoryWidget_MoveUp();
    void on_directoryWidget_MoveDown();
    void on_FindCombo_EnterPressed();

    void on_tableView_scrollChanged(int pos);
};

#endif // MAINWINDOW_H
