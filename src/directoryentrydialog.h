#ifndef DIRECTORYENTRYDIALOG_H
#define DIRECTORYENTRYDIALOG_H

#include <QDialog>

namespace Ui {
class DirectoryEntryDialog;
}

class DirectoryEntryDialog : public QDialog
{
    Q_OBJECT

    QString directory_;
    QString lastselecteddirectory_;
    QString displaytext_;

public:
    explicit DirectoryEntryDialog(QWidget *parent = nullptr);
    ~DirectoryEntryDialog();

private slots:
    void on_btnBrowse_clicked();
    void on_buttonBox_accepted();
    void on_tbBrowse_clicked();

private:
    Ui::DirectoryEntryDialog *ui;

protected:
    void showEvent(QShowEvent *ev);
    void updateData(const bool bDataToDialog);

public:
    QString directory() const {
        return directory_;
    }
    void setDirectory(const QString& dir) {
        directory_=dir;
    }
    void setLastSelectedDirectory(const QString& dir) {
        lastselecteddirectory_=dir;
    }

    QString displaytext() const {
        return displaytext_;
    }
    void setDisplayText(const QString displaytext) {
        displaytext_ = displaytext;
    }
};

#endif // DIRECTORYENTRYDIALOG_H
