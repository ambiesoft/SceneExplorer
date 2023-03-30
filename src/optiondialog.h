//SceneExplorer
//Exploring video files by viewer thumbnails
//
//Copyright (C) 2018  Ambiesoft
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H


#include "ui_option.h"

#include "globals.h"

class OptionDialog:public QDialog
{
    Q_OBJECT
    using ParentClass = QDialog;

private:
    Ui::Option ui;
    QString lastSelectedDir_;

    void on_setFileCommon(QLineEdit* le);

public:
    OptionDialog(QWidget* parent = nullptr);

    QString mainText_,subText_;
    ImageCacheType imagecache_;
    int maxgd_;
    int maxff_;
    int thumbCount_;
    int thumbWidth_, originalThumbWidth_;
    int thumbHeight_, originalThumbHeight_;
    QString thumbFormat_;
    QString scrollMode_;

    int taskPriority_= -1;
    int tagMenuFormat_;

    bool useCustomDBDir_;
    QString dbdir_;
    bool limitItems_;
    int maxRows_;
    bool openlastdoc_;
    QString ffprobe_, ffmpeg_;
    bool showtagcount_;

    virtual void accept() override;
private:
    void constructTitleTemplateMenu(QMenu& contextMenu,
                                    QList< QSharedPointer<QAction> >& acts,
                                    bool isMain);

protected:
    void showEvent(QShowEvent *) override;
private Q_SLOTS:
    void on_buttonBox_accepted();
    void on_tbDBDir_clicked();
    void on_tbffprobe_clicked();
    void on_tbffmpeg_clicked();
    void on_chkUseCustomDatabaseDirectory_stateChanged(int arg1);
    void on_chkLimitItems_stateChanged(int arg1);
    void on_tbInfoMain_clicked();
    void on_tbInfoSub_clicked();


    void on_context_titleTemplateCommonMain();
    void on_context_titleTemplateCommonSub();
private Q_SLOTS:
    virtual void done(int r) override;
};

#endif // OPTIONDIALOG_H
