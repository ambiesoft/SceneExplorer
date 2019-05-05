#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameDialog(QWidget *parent = nullptr);
    ~RenameDialog() override;

    QString basename() const;
    QString ext() const;

    void setBasename(const QString& basename);
    void setExt(const QString& ext);

    QString filename() const;
protected:
    void showEvent(QShowEvent *) override;
private Q_SLOTS:
    virtual void done(int r) override;



private:
    Ui::RenameDialog *ui;
};

#endif // RENAMEDIALOG_H
