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
#include <QThread>
#include <QThreadPool>

#include "../../lsMisc/stdQt/inisettings.h"
#include "../../lsMisc/stdosd/blockedbool.h"

#include "tablemodel.h"
#include "directoryentry.h"
#include "directoryitem.h"
#include "findcombobox.h"
#include "externaltoolitem.h"
#include "document.h"
#include "tagidsinfo.h"
#include "imainwindow.h"
#include "historylist.h"
#include "IFFTask2Main.h"
#include "consts.h"

class QThreadPool;
class QLabel;
class ListModel;
class TableItemData;
// class AmbiesoftQt::Settings;
class TaskModel;
class TaskFFmpeg;
class QLabel;
class QItemSelection;


typedef QSharedPointer<ExternalToolItem> ExternalToolItemPointer;

namespace Ui {
class MainWindow;
}

#define CHECK_DOCUMENT(doc) do { if(!doc) { Alert(this, TR_NO_DOCUMENT()); return;}} while(false)

class MainWindow : public QMainWindow, IMainWindow, IHistoryList, IFFTask2Main
{
    Q_OBJECT
    using ParentClass = QMainWindow;
    QString TR_NOVIDEO_SELECTED() { return tr("No Video Selected"); }
    QString TR_NODIRECTORY_SELECTED() { return  tr("No Directory Selected");}
    QString TR_ALLITEM_COULDNOTBE_COPIED() { return  tr("All item could not be copied.");}
    QString TR_MISSINGITEM_COULDNOTBE_COPIED() { return  tr("Missing item could not be copied.");}
    QString TR_SELECTEDITEM_NOT_NORMALITEM() { return  tr("Selected item is not a normal item.");}
    QString TR_NOTAG_SELECTED() { return tr("No Tag Selected"); }
    QString TR_NO_DOCUMENT() { return tr("No Document");}
    QString TR_CLIPBOARD_IS_EMPTY() { return tr("Clipboard is empty.");}
    QString TR_FAILED_TO_INSERT_DIRECTORY_INTO_DATABASE() { return tr("Failed to insert directory into Database.");}
    QString TR_FAILED_TO_INSERT_TAG_INTO_DATABASE() { return tr("Failed to insert tag into Database.");}
    QString TR_FAILED_TO_UPDATE_DIRECTORY_ON_DATABASE() { return tr("Failed to update directory on Database.");}

    Ui::MainWindow *ui;
    // setting keys
    AmbiesoftQt::IniSettings& settings_;
    Document* pDoc_ = nullptr;
    QStringList recents_;

    bool tableContextMenuActivaing_ = false;

    QScopedPointer<QThread::Priority> taskPriority_;

    int GetTaskPriorityAsInt();
    void SetTaskPriorityAsInt(int priority);

    HistoryList hisList_;
    bool bHistoryActivating_=false;

    QList<QString> lastScanErrors_;
public:
    explicit MainWindow(QWidget *parent,
                        AmbiesoftQt::IniSettings& settings);

    ~MainWindow();

    // History's virtual function
    void selectItem(const QString& movie) override;
    void updateToolButton() override;
    virtual QThread::Priority* GetTaskPriority() const override;
    bool IsInitialized() const;
    static bool isClosing() {
        return closing_;
    }
private:
    enum IDKIND {
        IDKIND_GetDir,
        IDKIND_CheckThumbs,
        IDKIND_FFmpeg,
    };
    bool initialized_ = false;
    static bool closing_;
    bool closed_ = false;
    bool directoryChanging_ = false;
    bool tagChanging_ = false;
    bool lastQueriedOnlyMissing_ = false;
    bool lastQueriedOnlyExistant_ = false;
    QStringList lastQueriedDirs_ = QStringList(QLatin1String("dummy"));
    QStringList lastQueriedTitles_ = QStringList(QLatin1String("dummy"));
    //    bool lastQueriedIsAllTag_ = false;
    //    QSet<qint64> lastQueriedTaggedIDs_ = {-1};
    TagidsInfo lastQueriedTaggedIds_;

    //SORTCOLUMN currentSort_ = SORTCOLUMN::SORT_NONE;
    //bool currentSortRev_ = false;
    void askRebootClose();
    QList<QAction*> getAllSortAction();
    void ScanSelectedDirectory(const bool bAll = false);
    bool CreateNewTag(const QString& tag,
                      const QString& yomi,
                      qint64* insertedTag=nullptr);
    QList<QWidget*> getAllStatusBarWidgets();
    void GetSelectedAndCurrentTagIDs(TagidsInfo& tagInfos);
    void GetSqlAllSetTable(const QStringList dirs,
                           const QStringList titles,
                           const TagidsInfo& tagInfos,
                           bool bOnlyMissing,
                           bool bOnlyExistant);
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
            Ambiesoft::BlockedBool bb(&blocking_);
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
            Ambiesoft::BlockedBool bb(&blocking_);
            cmb_->setCurrentIndex(0);
        }
        void SetIndexLast() {
            Ambiesoft::BlockedBool bb(&blocking_);
            cmb_->setCurrentIndex(cmb_->count()-1);
        }
        bool* GetBlockPointer() {
            return &blocking_;
        }
        bool IsBlocking() const {
            return blocking_;
        }
    };
    std::unique_ptr<LimitManager> limitManager_;

    class IDManager
    {
        MainWindow* win_;

        int idGetDir_ = 0;
        int idCheckThumbs_=0;
        int idFFMpeg_ = 0;

        int idGetDirDone_ = 0;
        int idCheckThumbsDone_=0;
        int idFFMpegDone_ = 0;

        void updateStatus();
    public:
        IDManager(MainWindow* win):win_(win){}

        int Get(IDKIND idkind) const
        {
            switch(idkind)
            {
            case IDKIND_GetDir: return idGetDir_;
            case IDKIND_CheckThumbs: return idCheckThumbs_;
            case IDKIND_FFmpeg: return idFFMpeg_;
            }
            Q_ASSERT(false);
            return 0;
        }
        int GetDone(IDKIND idkind) const
        {
            switch(idkind)
            {
            case IDKIND_GetDir: return idGetDirDone_;
            case IDKIND_CheckThumbs: return idCheckThumbsDone_;
            case IDKIND_FFmpeg: return idFFMpegDone_;
            }
            Q_ASSERT(false);
            return 0;
        }
        int Increment(IDKIND idkind)
        {
            int ret;
            switch(idkind)
            {
            case IDKIND_GetDir: ret = ++idGetDir_;break;
            case IDKIND_CheckThumbs: ret = ++idCheckThumbs_;break;
            case IDKIND_FFmpeg: ret = ++idFFMpeg_;break;
            }
            updateStatus();
            return ret;
        }
        int IncrementDone(IDKIND idkind)
        {
            int ret;
            switch(idkind)
            {
            case IDKIND_GetDir: ret = ++idGetDirDone_;break;
            case IDKIND_CheckThumbs: ret = ++idCheckThumbsDone_;break;
            case IDKIND_FFmpeg: ret = ++idFFMpegDone_;break;
            }
            updateStatus();
            return ret;
        }
        void Clear()
        {
            idGetDir_ = idGetDirDone_ = 0;
            idCheckThumbs_ = idCheckThumbsDone_ = 0;
            idFFMpeg_ = idFFMpegDone_=0;
            updateStatus();
        }
        bool isAllTaskFinished() const
        {
            if(
                    Get(IDKIND_FFmpeg)==GetDone(IDKIND_FFmpeg) &&
                    Get(IDKIND_CheckThumbs)==GetDone(IDKIND_CheckThumbs) &&
                    Get(IDKIND_GetDir)==GetDone(IDKIND_GetDir)
                    )
            {
                return true;
            }
            return false;
        }
    };
    std::unique_ptr<IDManager> idManager_;
    friend class IDManager;

    void checkTaskFinished();

    int optionThreadcountGetDir_ = 1;
    int optionThreadcountGetDir() const {
        return Consts::GetMaxMinedThreadCount(optionThreadcountGetDir_);
    }
    void setOptionThreadcountGetDir(int v) {
        optionThreadcountGetDir_ = v;
        if(pPoolGetDir_)
        {
            Q_ASSERT(optionThreadcountGetDir() > 0);
            pPoolGetDir_->setMaxThreadCount(optionThreadcountGetDir());
        }
    }

    int optionThreadcountThumbnail_ = 2;
    int optionThreadcountThumbnail() const {
        return Consts::GetMaxMinedThreadCount(optionThreadcountThumbnail_);
    }
    void setOptionThreadcountThumbnail(int v) {
        optionThreadcountThumbnail_ = v;
        if(pPoolFFmpeg_)
        {
            Q_ASSERT(optionThreadcountThumbnail() > 0);
            pPoolFFmpeg_->setMaxThreadCount(optionThreadcountThumbnail());
        }
    }
    int optionThumbCount_ = 3; // default is 3
    QString optionThumbFormat_ = QStringLiteral("jpg");
    int optionThumbWidth_ = 0;
    int optionThumbHeight_ = 0;

    int optionTagMenuFormat_ = 0;

    bool initShown_=false;
    bool IsClosed() const {
        return closed_;
    }

    QLabel *slFreeSpace_ = nullptr;
    void updateFreeSpace();

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
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void showEvent( QShowEvent* event ) override;
    bool event(QEvent *event) override;

    void RefreshDirectoryTree();
    void RefreshTagTree();
    void initLangMenus();
    class SortManager
    {
        SORTCOLUMNMY sort_;
        bool ascending_[COUNT_SORTCOLUMN];
        QIcon iconsAscend_[COUNT_SORTCOLUMN];
        QIcon iconsDescend_[COUNT_SORTCOLUMN];
        QAction* acs_[COUNT_SORTCOLUMN];
        QToolButton* tbs_[COUNT_SORTCOLUMN];
    public:
        SortManager();
        void onSort(SORTCOLUMNMY sc);
        void UpdateButtonText();
        SORTCOLUMNMY GetCurrentSort() const {
            return sort_;
        }
        bool IsSortAscending() const {
            return ascending_[sort_];
        }
        void setAction(SORTCOLUMNMY sc,
                       QAction* pAction);
        void setToolButton(SORTCOLUMNMY sc,
                           QToolButton* tb,
                           const QIcon& iconNormal,
                           const QIcon& iconRev);

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

    virtual SORTCOLUMNMY GetCurrentSort() override {
        return sortManager_.GetCurrentSort();
    }
    virtual QString GetTags(const qint64& id) override;
    QToolButton* tbLabelSort_ = nullptr;

    virtual int GetThumbWidth() override;
    virtual int GetThumbHeight() override;

    void on_ShowMissingClicked_common(bool nNextCheck);
    void checkAllTagCommon(const bool bCheck, const bool bSelection = false);


    void setFontCommon1(const QString& savekey,
                        QWidget* pWidget);
    void setFontCommon2(const QString& savekey,
                        std::function<QFont (void)> getfunc,
                        std::function<void(QFont&)> setfunc);
    QList<QWidget*> getAllDockingWindow();

    void createTagMenus(
        QMenu* pParentMenu,
        QList< QSharedPointer<QAction> >* actTagsKeeper,
        QList< QSharedPointer<QMenu> >* subMenusTagKeeper,
        QObject* pParent);


private Q_SLOTS:
    void editTag();
    void deleteTag();
    void OnCheckAllTag();
    void OnUncheckAllTag();
    void OnCheckSelectedTag();
    void OnUncheckSelectedTag();
    void showTagContextMenu(const QPoint&);
    void OnMenuStyle();
    void on_action_Close_triggered();
    void on_action_About_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_action_Options_triggered();
    void on_action_Pause_triggered();
    void on_action_Stop_triggered();
    void onUserDirectoryTriggered();
    void onArbitraryHistory();
    void onClearHistory();
    void onMenuTag_AboutToShow();
    void onMenuDirectory_AboutToShow();
    void onMenuTask_AboutToShow();
    void onMenuView_AboutToShow();
    void onMenuEdit_AboutToShow();
    void onMenuHistory_AboutToShow();
    void onMenuStyle_AboutToShow();
    void onMenuLanguage_AboutToShow();
    void onMenuDocking_windows_AboutToShow();
    
#ifdef QT_DEBUG
    void onDebugTest();
#endif

    void onMenu_RecentDocuments_AboutToShow();

    void on_actionSort_by_file_name_triggered();
    void on_actionSort_by_file_size_triggered();
    void on_actionSort_by_wtime_triggered();

    void on_tableView_customContextMenuRequested(const QPoint &pos);

    void OnDirectorySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void on_action_Top_triggered();

    void on_action_Bottom_triggered();

    void OndirectoryItemChanged(QListWidgetItem *item);
    void OnTableItemChanged(QModelIndex newIndex, QModelIndex oldIndex);


    void on_directoryWidget_customContextMenuRequested(const QPoint &pos);

    void on_actionSort_by_resolution_triggered();

    void on_actionSort_by_duration_triggered();

    void on_actionSort_by_bitrate_triggered();
    void OnShowMissingClicked();
    void on_action_Find_triggered();

    void on_action_ClearFind_triggered();

    void OnRecentDocumentsTriggered(bool checked=false);
    void OnClearRecentItemsTriggered(bool checked=false);

    // void onTaskTimerTick();
    void on_actionSort_by_open_count_triggered();



    void on_action_Extentions_triggered();

    void on_action_Font_triggered();

    void on_action_FontDetail_triggered();

    void on_action_Output_triggered();



    void on_action_Open_triggered();

    void on_action_Save_triggered();





    void on_action_New_triggered();


    void OnLimitFirstTriggered(bool checked=false);
    void OnLimitPrevTriggered(bool checked=false);
    void OnLimitNextTriggered(bool checked=false);
    void OnLimitLastTriggered(bool checked=false);
    void OnCmbLimitCurrentIndexChanged(int index);






    void on_actionSort_by_last_access_triggered();






    void on_action_DockDirectory_triggered();

    void on_action_DockTask_triggered();

    void on_action_DockOutput_triggered();



    void on_action_English_triggered();

    void on_action_Japanese_triggered();





    void on_action_Help_triggered();



    void on_action_DockTag_triggered();

    void on_listTag_itemSelectionChanged();



    void on_listTag_itemChanged(QListWidgetItem *item);

    void on_action_FontDirectory_triggered();

    void on_action_FontTask_triggered();

    void on_action_FontTag_triggered();

    void on_action_FontMenu_triggered();

    void on_action_FontDockingWindow_triggered();

    void on_action_FontStatusbar_triggered();

    void on_action_Refresh_triggered();

    void on_action_Copy_triggered();

    void on_action_Paste_triggered();



    void on_action_OpenVideo_triggered();

    void on_action_OpenDirectory_triggered();



    void on_action_ScanAllDirectories_triggered();



    void on_action_ScanSelectedDirectory_triggered();

    void on_actionSort_by_full_name_triggered();

    void on_action_ClearContent_triggered();

    void on_action_ScanArbitraryDirectory_triggered();

    void on_directoryWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listTag_itemDoubleClicked(QListWidgetItem *item);



    void on_action_FocusItemTable_triggered();






    void on_action_ShowAllItem_triggered();

    void on_action_SelectAll_triggered();

    void on_action_ShowMissingFiles_triggered();

    void on_action_ClearFindHistory_triggered();

    void on_action_FocusFind_triggered();

    void on_action_FocusDirectoryPane_triggered();

    void on_action_FocusTagPane_triggered();



    void on_action_AddNewTag_triggered();

    void on_action_ExternalTools_triggered();

    void on_action_SystemDefault_triggered();

    void on_action_AboutDocument_triggered();

    void on_action_CommandLine_triggered();

    void on_action_AboutQt_triggered();

    void on_action_AddDirectory_triggered();

    void on_action_Property_triggered();

    void on_txtLog_customContextMenuRequested(const QPoint &pos);

    void on_tableView_activated(const QModelIndex &index);

    void on_action_GoBack_triggered();

    void on_action_GoForward_triggered();

    void on_action_Rename_triggered();

    void on_action_Remove_triggered();







    void on_actionSort_by_fps_triggered();


    DirectoryItem* getDeepestDirectory(const QString& videoFile) const;
    bool isDeepestDirectorySelected(const QString& videoFile) const;
    void on_action_SelectDeepestDirectory_triggered();

    void on_action_SelectTags_triggered();

    void on_actionSort_by_format_triggered();

    void on_actionSort_by_vcodec_triggered();

    void on_actionSort_by_acodec_triggered();

    void on_actionSort_by_ctime_triggered();

    void on_actionShow_last_scan_error_triggered();

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

    void OnPasteDirectory();
    void OnPasteTag();


    QThreadPool* pPoolGetDir_ = nullptr;
    QThreadPool* getPoolGetDir();

    QThreadPool* pPoolCheckThumbs_ = nullptr;
    QThreadPool* getPoolCheckThumbs();

    QThreadPool* pPoolFFmpeg_ = nullptr;
    QThreadPool* getPoolFFmpeg();


    //    QThreadPool* pPoolFilter_ = nullptr;
    //    QThreadPool* getPoolFilter();

    void clearAllPool(bool bAppendLog=true);

//    void AddUserEntryDirectory_obsolete(
//            DirectoryItem::DirectoryItemType itemType,
//            const qint64& dirid,
//            const QString& dir,
//            bool sel,
//            bool check);
    bool HasDirectory(const QString& dir);
    void itemChangedCommon(bool bForceRead=false);

    void langChanged_common(const QString& lang);

    void StartScan(const QStringList& dirs);

    // QFileIconProvider fiProvider_;


    TableModel* tableModel_;
    QString lastSelectedAddDir_;
    QString lastSelectedScanDir_;
    QString lastSelectedDocumentDir_;

    TaskModel* taskModel_;

    void resizeDock_obsolete(QDockWidget* dock, const QSize& size);

    enum TaskKind {
        TaskKind_GetDir,
        TaskKind_CheckThumbs,
        TaskKind_CheckThumbsError,
        TaskKind_FFMpeg,
        TaskKind_FFMpegWarn,
        TaskKind_FFMpegError,
        TaskKind_SQL,
        TaskKind_SQLError,
        TaskKind_App,
    };
    bool IsErrorLogKind(TaskKind kind) const {
        return
                kind==TaskKind_CheckThumbsError ||
                kind==TaskKind_FFMpegWarn ||
                kind==TaskKind_FFMpegError ||
                kind==TaskKind_SQLError;
    }
    QStringList logPond_;
    int logTimerId_ = 0;
    int logTimerConsecutiveEmptyCount_=0;
    void insertLog(TaskKind kind, int id, const QString& text, bool bError=false);
    void insertLog(TaskKind kind, const QVector<int>& ids, const QStringList& texts, bool bError=false);
    void ResetLogTimer();

    void updateOnOpened(const qint64& id, const QString& movieFile);
    void openVideo(const qint64& id, const QString& movieFile);
    void openVideoDirectory(const QString& movieFile);
    QString getSelectedVideo(bool bNativeFormat = true);
    QString getVideoFromIndex(const QModelIndex& index, bool bNativeFormat);
    QString getUrlFromIndex(const QModelIndex& index);
    QString getMemoFromIndex(const QModelIndex& index);
    QModelIndex getSelectedIndex();
    qint64 getSelectedID();
    qint64 getIDFromIndex(const QModelIndex& index);

    bool IsAllTagSelected() const;
    bool IsDirSelected(const QString& dir) const;

    bool GetFFprobeVersion(QString& errString, QString& outString) const;
    bool GetFFmpegVersion(QString& errString, QString& outString) const;

    void CreateLimitManager();
    QString GetDefaultDocumentPath();

    bool OpenDocument(const QString& file, const bool bExists, const bool noHistory = false);
    bool CloseDocument();
    void InitDocument();
    bool LoadTags();
    void directoryWidgetMoveUpCommon(const bool bUp);
protected:
    void timerEvent(QTimerEvent *event) override;
public Q_SLOTS:
    //    void sayBorn(int id,
    //                   const QString& movieFile);
    void sayHello(int loopId, int id,
                  const QString& movieFile);
    void sayNo(int loopId, int id,
               const QString& movieFile,
               const QString& errorReason);
    void sayUpdated(int loopId, int id,
                    const qint64& recordid,
                   const QString& movieFile,
                   const double& duration,
                   const QString& format,
                   int bitrate,
                   const QString& vcodec,
                   const QString& acodec,
                   int vWidth,int vHeight,
                   const double& fps);
    void sayGoodby(int loopId, int id,
                   const QStringList& files,
                   const QString& movieFile,
                   const double& duration,
                   const QString& format,
                   int bitrate,
                   const QString& vcodec,
                   const QString& acodec,
                   int vWidth,int vHeight,
                   const QString& thumbext,
                   const double& fps);
    void finished_FFMpeg(int loopId, int id);
    void warning_FFMpeg(int loopId, int id,
                        const QString& warning);

    void afterGetDir(int loopId, int id,
                     const QString& dir,
                    const QList<MovieFileInfo>& mfis);
    void finished_GetDir(int loopId, int id ,const QString& dir);
    //    void afterFilter(int loopId, int id,
    //                     const QString& dir,
    //                     const QStringList& filteredFiles,
    //                     const QStringList& renameOlds,
    //                     const QStringList& renameNews);
    void afterFilter2(int loopId, int id,
                      const QString& dir,
                      const QStringList& filteredFiles,
                      const QList<QPair<qint64,QString>>& toUpdateFiles);


    //    void finished_Filter(int loopId, int id);

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


    void OnContextCopySelectedVideoFilename();
    void OnContextCopySelectedVideoFilenameWithoutExtension();
    void OnContextCopyFolderName();
    void OnRemove();
    void OnProperty();
    bool HasUrl(const QModelIndex& index);
    void OnOpenUrl();
    void OnRename();
    void OnContextExternalTools();
    void OnContextAddTags();

    void tableItemCountChanged();
    void tableSortParameterChanged(SORTCOLUMNMY sc, bool rev);


    void OnDirectoryRemove();
    void OnDirectoryRemoveMissingItems();
    void OnDirectoryProperty();
    void CheckDirectoryCommon(const bool bCheck, const bool bSelection);
    void OnDirectoryCheckAll();
    void OnDirectoryUncheckAll();
    void OnDirectoryCheckSelection();
    void OnDirectoryUncheckSelection();
    void OnDirectorySortByDirectory();
    void OnDirectorySortByText();
    void OnDirectoryMoveUp();
    void OnDirectoryMoveDown();
    void OnFindComboEnterPressed();

    void OnTableViewScrollChanged(int pos);

    void OnTxtLogCopy();
    void OnTxtLogCopyAll();
    void OnTxtLogWrap(bool b);
    void OnTxtLogClear();
};

#endif // MAINWINDOW_H
