#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tablemodel.h"

class QThreadPool;
class ListModel;
class ItemData;
class TreeModel;
class Settings;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, Settings& settings);
    ~MainWindow();

private:
    int idGetDir_ = 0;
    int idFFMpeg_ = 0;

    int threadcountGetDir_ = 1;
    int threadcountFFmpeg_ = 2;
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

private:
    QThreadPool* poolFFMpeg_;
    QThreadPool* poolGetDir_;

    Ui::MainWindow *ui;
    TableModel* tableModel_;
    QString lastSelectedDir_;

    TreeModel* treeModel_;

    void resizeDock(QDockWidget* dock, const QSize& size);

    enum TaskKind {
        GetDir,
        FFMpeg,
    };
    void insertLog(TaskKind kind, int id, const QString& text);

public slots:
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

    void afterGetDir(int id,
                     const QStringList& dirs);

    void onMenuEdit_AboutToShow();
};

#endif // MAINWINDOW_H
