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

#ifndef CONSTS_H
#define CONSTS_H

namespace Consts
{
Q_CONSTEXPR char ORGANIZATION[] = "Ambiesoft";
Q_CONSTEXPR char APPDOMAIN[] = "ambiesoft.fam.cx";
Q_CONSTEXPR char APPNAME[] = "SceneExplorer";
Q_CONSTEXPR char APPVERSION[] = "1.20.1";


Q_CONSTEXPR char KEY_STYLE[] = "style";
Q_CONSTEXPR char KEY_LANGUAGE[] = "language";


Q_CONSTEXPR char KEY_GEOMETRY[] = "geometry";
Q_CONSTEXPR char KEY_WINDOWSTATE[] = "windowState";

Q_CONSTEXPR char KEY_SIZE[] = "size";
Q_CONSTEXPR char KEY_TREESIZE[] = "treesize";
Q_CONSTEXPR char KEY_TXTLOGSIZE[] = "txtlogsize";
Q_CONSTEXPR char KEY_LISTTASKSIZE[] = "listtasksize";


Q_CONSTEXPR char KEY_COMBO_FINDTEXTS[] = "combofindtexts";
Q_CONSTEXPR char KEY_SHOWMISSING[] = "showmissing";

Q_CONSTEXPR char KEY_TXTLOG_WRAP[] = "txtlogwrap";

Q_CONSTEXPR char KEY_LASTSELECTEDADDDIRECTORY[] = "lastselectedadddir";
Q_CONSTEXPR char KEY_LASTSELECTEDSCANDIRECTORY[] = "lastselectedscandir";
Q_CONSTEXPR char KEY_LASTSELECTEDDOCUMENT[] = "lastselecteddocumentdir";

Q_CONSTEXPR char KEY_USE_CUSTOMDATABASEDIR[] = "usecustomdatabasedir";
Q_CONSTEXPR char KEY_DATABASE_PATH[] = "databasepath";





Q_CONSTEXPR char KEY_MAX_GETDIR_THREADCOUNT[] = "maxgetdirthreadcount";
Q_CONSTEXPR char KEY_MAX_THUMBNAIL_THREADCOUNT[] = "maxthumbnailthreadcount";
Q_CONSTEXPR char KEY_THUMBNAIL_COUNT[] = "thumbcount";
Q_CONSTEXPR char KEY_THUMBNAIL_FORMAT[] = "thumbformat";
Q_CONSTEXPR char KEY_THUMBNAIL_SCROLLMODE[] = "thumbscrollmode";

Q_CONSTEXPR char KEY_TASK_PRIORITY[] = "taskpriority";

Q_CONSTEXPR char KEY_TITLE_TEXT_TEMPLATE[] = "titletexttemplate";
Q_CONSTEXPR char KEY_INFO_TEXT_TEMPLATE[] = "infotexttemplate";

Q_CONSTEXPR char KEY_IMAGECACHETYPE[] = "imagecachetype";


Q_CONSTEXPR char KEY_EXTENSION_ORDERALLOW[] = "extensionoderallow";
Q_CONSTEXPR char KEY_ALLOW_EXTENSIONS[] = "allowextensions";
Q_CONSTEXPR char KEY_DENY_EXTENSIONS[] = "denyextensions";

Q_CONSTEXPR char KEY_FONT_TABLEINFO[] = "fonttableinfo";
Q_CONSTEXPR char KEY_FONT_TABLEDETAIL[] = "fonttabledetail";
Q_CONSTEXPR char KEY_FONT_OUPUT[] = "fontoutput";
Q_CONSTEXPR char KEY_FONT_DIRECTORY[] = "fontdirectory";
Q_CONSTEXPR char KEY_FONT_TASK[] = "fonttask";
Q_CONSTEXPR char KEY_FONT_TAG[] = "fonttag";
Q_CONSTEXPR char KEY_FONT_MENU[] = "fontmenu";
Q_CONSTEXPR char KEY_FONT_STATUSBAR[] = "fontstatusbar";
Q_CONSTEXPR char KEY_FONT_DOCKINGWINDOW[] = "fontdockingwindow";

Q_CONSTEXPR char KEY_EXTERNALTOOLS_LASTSELECTEDEXEDIR[] = "externaltoolslastselectedexedir";

Q_CONSTEXPR char KEY_EXTERNALTOOLS_COUNT[] = "externaltoolscount";
Q_CONSTEXPR char KEY_EXTERNALTOOLS_GROUPPRIX[] = "externaltool_";
Q_CONSTEXPR char KEY_EXTERNALTOOLS_NAME[] = "externaltoolname";
Q_CONSTEXPR char KEY_EXTERNALTOOLS_EXE[] = "externaltoolexe";
Q_CONSTEXPR char KEY_EXTERNALTOOLS_ARG[] = "externaltoolarg";
Q_CONSTEXPR char KEY_EXTERNALTOOLS_COUNTASOPEN[] = "externaltoolcountasopen";


Q_CONSTEXPR char KEY_RECENT_OPENDOCUMENTS[] = "recentdocuments";
Q_CONSTEXPR char KEY_SORT[] = "sort";
Q_CONSTEXPR char KEY_SORTREV[] = "sortrev";

Q_CONSTEXPR char KEY_LIMIT_ITEMS[] = "limititems";
Q_CONSTEXPR char KEY_LIMIT_NUMBEROFROWS[] = "limitnumberofrows";

Q_CONSTEXPR char KEY_OPEN_LASTOPENEDDOCUMENT[] = "openlastdocument";

Q_CONSTEXPR char KEY_FFPROBE_EXECUTABLE[] = "ffprobeexecutable";
Q_CONSTEXPR char KEY_FFMPEG_EXECUTABLE[] = "ffmpegexecutable";

Q_CONSTEXPR char KEY_MESSAGEBOX_REMOVEFORMEXTERNALMEDIA[] = "mb_removefromexternalmedia";


Q_CONSTEXPR char FILEPART_THUMBS[] = "thumbs";

Q_CONSTEXPR int THUMB_WIDTH=240;
Q_CONSTEXPR int THUMB_HEIGHT=180;

Q_CONSTEXPR int UUID_LENGTH=36;
// Q_CONSTEXPR int THUMB_FILENAME_LENGTH=42;

Q_CONSTEXPR char DEFAULT_ITEM_MAIN_TEXT[] = "${name}";
Q_CONSTEXPR char DEFAULT_ITEM_SUB_TEXT[] = "${size}";

Q_CONSTEXPR int MAX_COMBOFIND_SAVECOUNT = 256;

Q_CONSTEXPR char STR_ENV_SCENEEXPLORER_ROOT[] = "${SCENEEXPLORER_ROOT}";

Q_CONSTEXPR float FLOAT1000 = 1000.0f;
Q_CONSTEXPR float FLOAT1024 = 1024.0f;
}
#endif // CONSTS_H
