#include "taginputdialog.h"
#include "ui_taginputdialog.h"
#include "blockedbool.h"

TagInputDialog::TagInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagInputDialog)
{
    ui->setupUi(this);
}

TagInputDialog::~TagInputDialog()
{
    delete ui;
}

void TagInputDialog::on_lineTag_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(!yomiChanged_)
    {
        if(lastTagText_==ui->lineYomi->text())
        {
            BlockedBool bb(&yomiChangging_);
            ui->lineYomi->setText(ui->lineTag->text());
        }
    }
    lastTagText_ = ui->lineTag->text();
}

void TagInputDialog::on_lineYomi_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(yomiChangging_)
        return;

    yomiChanged_ = true;
}

QString TagInputDialog::tag() const
{
    return ui->lineTag->text();
}
QString TagInputDialog::yomi() const
{
    return ui->lineYomi->text();
}
