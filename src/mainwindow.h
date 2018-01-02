#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imagemodel.h"

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

private slots:
    void on_action_Close_triggered();

    void on_action_Do_It_triggered();

private:
    Ui::MainWindow *ui;
    ImageModel* imageModel_;
public slots:
    void sayGoodby(int id,const QStringList& files);
};

#endif // MAINWINDOW_H
