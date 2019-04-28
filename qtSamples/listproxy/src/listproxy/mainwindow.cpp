#include <QDirModel>
#include "mydirmodel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MyDirModel* model = new MyDirModel(this);
    model->setReadOnly(true);
    model->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    ui->treeView->setModel(model);

}

MainWindow::~MainWindow()
{
    delete ui;
}
