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

#ifndef OSD_H
#define OSD_H

#include <QThread>

bool showInGraphicalShell(QWidget *parent, const QString &pathIn);
void MoveToTrashImpl( QString file );

QString GetDefaultFFprobe();
QString GetDefaultFFmpeg();

QString getInifile(bool& bExit);

bool isLegalFilePath(QString filename, QString* pError);
QString GetIllegalFilenameCharacters();

bool setProcessPriority(const qint64& pid, QThread::Priority priority, QStringList& errors);

bool myRename(const QString& oldfull, const QString& newfull, QString& error);

QString GetUserDocumentDirectory();

bool StartProcessDetached(const QString& exe, const QString& arg);

QIcon GetIconFromExecutable(const QString& exe);








#endif // OSD_H
