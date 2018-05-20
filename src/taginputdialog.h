#ifndef TAGINPUTDIALOG_H
#define TAGINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class TagInputDialog;
}

class TagInputDialog : public QDialog
{
    Q_OBJECT

    QString lastTagText_;
    bool yomiChangging_=false;
    bool yomiChanged_ = false;
public:
    explicit TagInputDialog(QWidget *parent = 0);
    ~TagInputDialog();

    QString tag() const;
    void setTag(const QString& txt);

    QString yomi() const;
    void setYomi(const QString& txt);

private slots:
    void on_lineTag_textChanged(const QString &arg1);

    void on_lineYomi_textChanged(const QString &arg1);

private:
    Ui::TagInputDialog *ui;
};

#endif // TAGINPUTDIALOG_H
