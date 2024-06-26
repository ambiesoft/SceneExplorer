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

#include "../../lsMisc/stdQt/stdQt.h"

#include "tagitem.h"
#include "mycontextmenu.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

using namespace Consts;
using namespace AmbiesoftQt;

void MainWindow::createTagMenus(
    QMenu* pParentMenu,
    QList< QSharedPointer<QAction> >* actTagsKeeper,
    QList< QSharedPointer<QMenu> >* subMenusTagKeeper,
    QObject* pParent)
{
    QList<QSharedPointer<TagItem>> tags;
    pDoc_ && pDoc_->GetAllTags(tags, false);
    QSet<qint64> tagsCurrent;
    pDoc_ && pDoc_->GetTagsFromID(getSelectedID(), tagsCurrent);
    if(tags.isEmpty())
    {
        QAction* act = new QAction(tr("No tags"), pParent);
        QSharedPointer<QAction> sact;
        if(actTagsKeeper) {
            sact.reset(act);
            actTagsKeeper->append(sact);
        }
        act->setEnabled(false);
        pParentMenu->addAction(act);
    }
    else
    {
        if(optionTagMenuFormat_==0)
        {
            for(auto&& ti : tags)
            {
                QString text = ti->tagtext();
                QAction* act = new QAction(text, pParent);
                QSharedPointer<QAction> sact;
                if(actTagsKeeper)
                {
                    sact.reset(act);
                    actTagsKeeper->append(sact);
                }
                connect(act, SIGNAL(triggered()),
                        this, SLOT(OnContextAddTags()));
                pParentMenu->addAction(act);
                act->setData(ti->tagid());
                act->setCheckable(true);
                if (tagsCurrent.contains(ti->tagid()))
                    act->setChecked(true);
            }
        }
        else if(optionTagMenuFormat_==1 || optionTagMenuFormat_==2)
        {
            std::map<QChar, std::vector<TagItem*>> charToTag;
            for(auto&& ti : tags)
            {
                if(!ti->IsNormalItem())
                    continue;

                QChar firstChar = getFirstCharcter(
                            optionTagMenuFormat_==1 ? ti->tagtext() : ti->yomi(), ' ');
                charToTag[firstChar].push_back(ti.data());
            }

            std::list<QChar> keysList;
            for (const auto& pair : charToTag){
                keysList.push_back(pair.first);
            }

            keysList.sort();

            for(auto&& firstChar : keysList)
            {
                QMenu* subMenu = new QMenu(QString(firstChar));
                QSharedPointer<QMenu> ssubMenu;
                if(subMenusTagKeeper) {
                    ssubMenu.reset(subMenu);
                    subMenusTagKeeper->append(ssubMenu);
                }
                for(auto&& ti : charToTag[firstChar])
                {
                    QString text = ti->tagtext();

                    // passing this makes |action| delete when |this| is deleting
                    // Deleting |action| manually cause stop above action, I believe.
                    QAction* action =  new QAction(text, pParent);
                    QSharedPointer<QAction> saction;
                    if(actTagsKeeper ){
                        saction.reset(action);
                        actTagsKeeper->append(saction);
                    }

                    if (tagsCurrent.contains(ti->tagid())) {
                        static QIcon iconSubMenu(":resource/images/tagmenu-check.png");
                        subMenu->setIcon(iconSubMenu);
                        action->setIcon(iconSubMenu);
                    }

                    action->setData(ti->tagid());
                    connect(action, SIGNAL(triggered()),
                            this, SLOT(OnContextAddTags()));
                    subMenu->addAction(action);
                }
                pParentMenu->addMenu(subMenu);
            }
        }
    }
}

void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    if(ui->tableView->selectionModel()->hasSelection())
    {
        Ambiesoft::BlockedBool bb(&tableContextMenuActivaing_);

        MyContextMenu contextMenu("TableView Context Menu", this);

        contextMenu.addEnablingAction(ui->action_OpenVideo);
        contextMenu.addEnablingAction(ui->action_OpenDirectory);

        contextMenu.addSeparator();

        // external tools ----->
        QMenu menuExternalTools(tr("External &tools..."), this);
        QList< QSharedPointer<QAction> > actExts;
        if(externalTools_.isEmpty())
        {
            QSharedPointer<QAction> act(new QAction(tr("No exteral tools")));
            act->setEnabled(false);
            menuExternalTools.addAction(act.data());
            actExts.append(act);
        }
        else
        {
            for(int i=0 ; i < externalTools_.count(); ++i)
            {
                QSharedPointer<QAction> act(new QAction(externalTools_[i].GetName()));
                act->setIcon(externalTools_[i].GetIcon());
                connect(act.data(), SIGNAL(triggered()),
                        this, SLOT(OnContextExternalTools()));
                menuExternalTools.addAction(act.data());
                act->setData(i);
                actExts.append(act);
            }
        }
        menuExternalTools.addSeparator();
        menuExternalTools.addAction(ui->action_ExternalTools);

        contextMenu.addMenu(&menuExternalTools);
        // <---- external tools

        contextMenu.addSeparator();

        contextMenu.addEnablingAction(ui->action_Copy);

        // sub menu start --->
        QMenu menuCopyOther(tr("C&opy others..."), this);

        QAction actionCopyFilename(tr("&Filename"));
        connect(&actionCopyFilename, SIGNAL(triggered()),
                this, SLOT(OnContextCopySelectedVideoFilename()));
        menuCopyOther.addAction(&actionCopyFilename);

        QAction actionCopyFilenameWithoutExtension(tr("Filename without &extension"));
        connect(&actionCopyFilenameWithoutExtension, SIGNAL(triggered()),
                this, SLOT(OnContextCopySelectedVideoFilenameWithoutExtension()));
        menuCopyOther.addAction(&actionCopyFilenameWithoutExtension);

        menuCopyOther.addSeparator();

        QAction actionCopyFolderName(tr("Fo&lder name"));
        connect(&actionCopyFolderName, SIGNAL(triggered()),
                this, SLOT(OnContextCopyFolderName()));
        menuCopyOther.addAction(&actionCopyFolderName);

        contextMenu.addMenu(&menuCopyOther);
        // <--- sub menu end


        contextMenu.addSeparator();
        // select deepest directory
        ui->action_SelectDeepestDirectory->setChecked(isDeepestDirectorySelected(getSelectedVideo()));
        contextMenu.addAction(ui->action_SelectDeepestDirectory);
        contextMenu.addAction(ui->action_SelectTags);

        contextMenu.addSeparator();

        QAction actionRename(tr("Re&name"));
        connect(&actionRename, SIGNAL(triggered()),
                this, SLOT(OnRename()));
        contextMenu.addAction(&actionRename);


        QAction actionRemoveFromDB(tr("&Remove"));
        connect(&actionRemoveFromDB, SIGNAL(triggered()),
                this, SLOT(OnRemove()));
        contextMenu.addAction(&actionRemoveFromDB);

        contextMenu.addSeparator();

        // Tags tools ----->
        QMenu menuAddTags(tr("Ta&g..."), this);
        QList< QSharedPointer<QAction> > actTagsKeeper;
        QList< QSharedPointer<QMenu> > subMenusTagKeeper;
        createTagMenus(&menuAddTags, &actTagsKeeper, &subMenusTagKeeper, nullptr);

        menuAddTags.addSeparator();
        menuAddTags.addAction(ui->action_AddNewTag);
        contextMenu.addMenu(&menuAddTags);
        // <---- Tags


        contextMenu.addSeparator();

        // ---> Open URL
        QAction actionItemOpenUrl(tr("Open &Url"));
        actionItemOpenUrl.setEnabled(HasUrl(getSelectedIndex()));
        connect(&actionItemOpenUrl, SIGNAL(triggered()),
                this, SLOT(OnOpenUrl()));
        contextMenu.addAction(&actionItemOpenUrl);
        // <--- OpenURL

        contextMenu.addSeparator();

        // ---> Property
        QAction actionItemProperty(tr("&Property..."));
        connect(&actionItemProperty, SIGNAL(triggered()),
                this, SLOT(OnProperty()));
        contextMenu.addAction(&actionItemProperty);
        // <--- Property


        contextMenu.exec(ui->tableView->mapToGlobal(pos));
    }
    else
    {

    }
}


