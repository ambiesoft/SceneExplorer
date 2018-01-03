#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tablemodel.h"

class QThreadPool;
class ListModel;
class ItemData;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_Close_triggered();
    void on_action_Do_It_triggered();

    void on_action_About_triggered();

private:
    QThreadPool* pool_;
    Ui::MainWindow *ui;
    TableModel* imageModel_;
    QString lastSelectedDir_;

//    ListModel* listModel_;

public:
//    int getItemCount() const {
//        return listItems_.count();
//    }
//    QString getItemMovieFile(int index) const ;
public slots:
    void sayGoodby(int id,
                   const QStringList& files,
                   int width,
                   int height,
                   const QString& movieFile,
                   const QString& format);
};

#endif // MAINWINDOW_H
