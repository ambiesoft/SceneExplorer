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

#include "../../lsMisc/stdQt/inisettings.h"

#include "osd.h"
#include "consts.h"
#include "externaltoolitem.h"

using namespace Consts;
using namespace AmbiesoftQt;

ExternalToolItem ExternalToolItem::Load(int i, IniSettings& settings)
{
    QString group = KEY_EXTERNALTOOLS_GROUPPRIX + QString::number(i);

    settings.beginGroup(group);
    QString name = settings.valueString(KEY_EXTERNALTOOLS_NAME);
    QString exe = settings.valueString(KEY_EXTERNALTOOLS_EXE);
    QString arg = settings.valueString(KEY_EXTERNALTOOLS_ARG);
    bool bCountAsOpen = settings.valueBool(KEY_EXTERNALTOOLS_COUNTASOPEN);
    settings.endGroup();

    return ExternalToolItem(name,exe,arg,bCountAsOpen);
}
void ExternalToolItem::Save(int i, IniSettings& settings)
{
    QString group = KEY_EXTERNALTOOLS_GROUPPRIX + QString::number(i);

    settings.beginGroup(group);
    settings.setValue(KEY_EXTERNALTOOLS_NAME, name_);
    settings.setValue(KEY_EXTERNALTOOLS_EXE, exe_);
    settings.setValue(KEY_EXTERNALTOOLS_ARG, arg_);
    settings.setValue(KEY_EXTERNALTOOLS_COUNTASOPEN, countAsOpen_);
    settings.endGroup();
}

void ExternalToolItem::SetExe(const QString& exe) {
    if(exe_==exe)
        return;
    exe_ = exe;
    if(exe.isEmpty())
        icon_ = QIcon();
    else
        icon_ = GetIconFromExecutable(exe);
}
