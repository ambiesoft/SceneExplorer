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
// #include <QFileIconProvider>
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

#define CHECK_DOCUMENT(doc) do { if(!doc) { Alert(this, TR_NO_DOCUMENT()); return;}} while(false)

class MainWindow : public QMainWindow, IMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;
    // setting keys
    Settings& settings_;
    Document* pDoc_ = nullptr;
    QStringList recents_;

    QString TR_NOVIDEO_SELECTED() { return tr("No Video Selected"); }
    QString TR_NODIRECTORY_SELECTED() { return  tr("No Directory Selected");}
    QString TR_ALLITEM_COULDNOTBE_COPIED() { return  tr("All item could not be copied.");}
    QString TR_MISSINGITEM_COULDNOTBE_COPIED() { return  tr("Missing item could not be copied.");}
    QString TR_SELECTEDITEM_NOT_NORMALITEM() { return  tr("Selected item is not a normal item.");}
    QString TR_NOTAG_SELECTED() { return tr("No Tag Selected"); }
    QString TR_NO_DOCUMENT() { return tr("No Document");}
public:
    explicit MainWindow(QWidget *parent,
                        Settings& settings,
                        const QString& docToOpen);
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
    bool tagChanging_ = false;
    QStringList currentDirs_ = {"dummy"};
    bool currentIsTagValid_ = false;
    QSet<qint64> currentTaggedIDs_ = {-1};

    //SORTCOLUMN currentSort_ = SORTCOLUMN::SORT_NONE;
    //bool currentSortRev_ = false;

    QList<QWidget*> getAllStatusBarWidgets();
    bool GetSelectedTagIDs(QSet<qint64>& taggedids);
    void GetSqlAllSetTable(const QStringList& dirs,
                           QSet<qint64>* pTagged,
                           bool bOnlyMissing = false);
    QToolButton* tbShowNonExistant_ = nullptr;

    enum UpdateTitleType { DEFAULT, ONLYMISSING, INIT };
    void UpdateTitle(const QStringList& dirs, UpdateTitleType utt);

    QList<ExternalToolItem> externalTools_;

    void StoreDocument();

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


    void setMenuFont(QFont& font);

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
    virtual QString GetTags(const qint64& id);
	QToolButton* tbLabelSort_ = nullptr;
	
    void on_ShowMissingClicked_common(bool nNextCheck);
    void checkAllTagCommon(const bool bCheck);


    void setFontCommon1(const QString& savekey,
                        QWidget* pWidget);
    void setFontCommon2(const QString& savekey,
                        std::function<QFont (void)> getfunc,
                        std::function<void(QFont&)> setfunc);
    QList<QWidget*> getAllDockingWindow();


private slots:
    void editTag();
    void deleteTag();
    void checkAllTag();
    void uncheckAllTag();
    void showTagContextMenu(const QPoint&);
    void on_action_Close_triggered();
    void on_action_About_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_action_Options_triggered();
    void on_action_Pause_triggered();
    void on_action_Stop_triggered();
    void on_menu_directory_triggered();
    void on_menu_tag_triggered();
    void onMenuTag_AboutToShow();
    void onMenuFolder_AboutToShow();
    void onMenuTask_AboutToShow();
    void onMenuEdit_AboutToShow();
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
    void on_ClearRecentItems_triggered(bool checked=false);

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

    void on_action_Help_triggered();

    void on_action_Add_new_tag_triggered();

    void on_action_DockTag_triggered();

    void on_listTag_itemSelectionChanged();

    void on_actionShow_All_Item_triggered();

    void on_listTag_itemChanged(QListWidgetItem *item);

    void on_action_FontFolder_triggered();

    void on_action_FontTask_triggered();

    void on_action_FontTag_triggered();

    void on_action_FontMenu_triggered();

    void on_action_FontDockingWindow_triggered();

    void on_action_FontStatusbar_triggered();

    void on_action_Refresh_triggered();

    void on_action_Copy_triggered();

    void on_action_Paste_triggered();

    void on_action_Select_All_triggered();

    void on_action_OpenVideo_triggered();

    void on_action_OpenFolder_triggered();



private:
    void OnCopyTable();
    void OnCopyDirectory();
    void OnCopyTask();
    void OnCopyLog();
    void OnCopyTag();

    void OnUpdateEditCopy(QAction* pAction);

    void OnUpdateCopyTable(QAction* pAction);
    void OnUpdateCopyDirectory(QAction* pAction);
    void OnUpdateCopyTask(QAction* pAction);
    void OnUpdateCopyLog(QAction* pAction);
    void OnUpdateCopyTag(QAction* pAction);



    QThreadPool* pPoolFFmpeg_ = nullptr;
    QThreadPool* getPoolFFmpeg();

    QThreadPool* pPoolGetDir_ = nullptr;
    QThreadPool* getPoolGetDir();

//    QThreadPool* pPoolFilter_ = nullptr;
//    QThreadPool* getPoolFilter();

    void clearAllPool(bool bAppendLog=true);

    void AddUserEntryDirectory(
            DirectoryItem::DirectoryItemType itemType,
            const qint64& dirid,
            const QString& dir,
            bool sel,
            bool check);
    bool HasDirectory(const QString& dir);
    void itemChangedCommon(bool bForceRead=false);

    void langChanged_common(const QString& lang);

    void StartScan(QListWidgetItem* item);
    void StartScan(const QString& dir);

    // QFileIconProvider fiProvider_;


    TableModel* tableModel_;
	FileMissingFilterProxyModel *proxyModel_;
    QString lastSelectedAddFolderDir_;
    QString lastSelectedScanDir_;
    QString lastSelectedDocumentDir_;

    TaskModel* taskModel_;

    void resizeDock(QDockWidget* dock, const QSize& size);

    enum TaskKind {
        GetDir,
        FFMpeg,
        SQL,
        App,
    };
    void insertLog(TaskKind kind, int id, const QString& text, bool bError=false);
    void insertLog(TaskKind kind, const QVector<int>& ids, const QStringList& texts, bool bError=false);
    void updateOnOpened(const qint64& id, const QString& movieFile);
    void openVideo(const qint64& id, const QString& movieFile);
    void openVideoInFolder(const QString& movieFile);
    QString getSelectedVideo(bool bNativeFormat = true);
    qint64 getSelectedID();

    bool IsDirSelected(const QString& dir) const;

    bool checkExeCommon(const QString& exe, QString& errString) const;
    bool checkFFprobe(QString& errString) const;
    bool checkFFmpeg(QString& errString) const;

    void CreateLimitManager();
    QString GetDefaultDocumentPath();

    bool OpenDocument(const QString& file, const bool bExists);
    bool CloseDocument();
    void InitDocument();
    void LoadTags();
    void directoryWidgetMoveUpCommon(const bool bUp);
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




    void on_context_copySelectedVideoFilename();
    void on_context_removeFromDatabase();
    void on_context_Rename();
    void on_context_ExternalTools();
    void on_context_AddTags();

    void tableItemCountChanged();
    void tableSortParameterChanged(SORTCOLUMNMY sc, bool rev);

	void on_Rescan();
	void on_directoryWidget_Remove();
	void on_directoryWidget_RemoveMissingItems();
    void on_directoryWidget_CheckAll();
    void on_directoryWidget_UncheckAll();
    void on_directoryWidget_SortByName();
	void on_directoryWidget_MoveUp();
    void on_directoryWidget_MoveDown();
    void on_FindCombo_EnterPressed();

    void on_tableView_scrollChanged(int pos);
};

#endif // MAINWINDOW_H
