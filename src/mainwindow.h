#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tablemodel.h"

class QThreadPool;
class ListModel;
class TableItemData;
class FolderModel;
class Settings;
class TaskModel;
class TaskFFmpeg;
class QLabel;

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
    int idGetDir_ = 0;
    int idFFMpeg_ = 0;

    int threadcountGetDir_ = 1;
    int threadcountFFmpeg_ = 2;

    bool initShown=false;

    QLabel *statusLabel_ = nullptr;
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

private:
    QThreadPool* pPoolFFmpeg_ = nullptr;
    QThreadPool* getPoolFFmpeg();
    void clearPoolFFmpeg();

    QThreadPool* pPoolGetDir_ = nullptr;
    QThreadPool* getPoolGetDir();
    void clearPoolGetDir();

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
    void sayHello(int id,
                   const QString& movieFile);
    void sayNo(int id,
                   const QString& movieFile);
    void sayGoodby(int id,
                   const QStringList& files,
                   const QString& movieFile,
                   int thumbwidth,
                   int thumbheight,
                   const double& duration,
                   const QString& format,
                   const QString& vcodec,
                   const QString& acodec,
                   int vWidth,int vHeight);
    void sayDead(int id);

    void afterGetDir(int id,
                     const QString& dir,
                     const QStringList& files);
    void afterFilter(int id,
                     const QString& dir,
                     const QStringList& filteredFiles,
                     const QStringList& renameOlds,
                     const QStringList& renameNews);

    void openSelectedVideo();
    void openSelectedVideoInFolder();
    void copySelectedVideoPath();
};

#endif // MAINWINDOW_H
