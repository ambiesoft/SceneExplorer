#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

#include "../../lsMisc/stdQt/inisettings.h"

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
    Q_OBJECT

    AmbiesoftQt::IniSettings& settings_;

public:
    explicit RenameDialog(AmbiesoftQt::IniSettings& settings, QWidget *parent = nullptr);
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



    void on_buttonBox_accepted();

private:
    Ui::RenameDialog *ui;
};

#endif // RENAMEDIALOG_H
