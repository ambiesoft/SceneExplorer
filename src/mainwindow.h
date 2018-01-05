#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tablemodel.h"

class QThreadPool;
class ListModel;
class TableItemData;
class TreeModel;
class Settings;
class TaskModel;
class TaskFFMpeg;

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

//    QTimer* timer_ = nullptr;

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


private:
    QThreadPool* poolFFMpeg_ = nullptr;

    QThreadPool* pPoolGetDir_ = nullptr;
    QThreadPool* getPoolGetDir();
    void clearPoolGetDir();
    void clearAllPool();

    Ui::MainWindow *ui;
    TableModel* tableModel_;
    QString lastSelectedDir_;

    TreeModel* treeModel_;

    TaskModel* taskModel_;

    void resizeDock(QDockWidget* dock, const QSize& size);

    enum TaskKind {
        GetDir,
        FFMpeg,
    };
    void insertLog(TaskKind kind, int id, const QString& text);

public slots:
//    void sayBorn(int id,
//                   const QString& movieFile);
    void sayHello(int id,
                   const QString& movieFile);
    void sayNo(int id,
                   const QString& movieFile);
    void sayGoodby(int id,
                   const QStringList& files,
                   int width,
                   int height,
                   const QString& movieFile,
                   const QString& format);
    void sayDead(int id);

    void afterGetDir(int id,
                     const QStringList& dirs);

//	void OnTimer();
};

#endif // MAINWINDOW_H
