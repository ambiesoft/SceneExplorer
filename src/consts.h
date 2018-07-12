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
    const char ORGANIZATION[] = "Ambiesoft";
    const char APPDOMAIN[] = "ambiesoft.fam.cx";
    const char APPNAME[] = "SceneExplorer";
    const char APPNAME_DISPLAY[] = "Scene Explorer";
    const char APPVERSION[] = "1.17";


    const char KEY_STYLE[] = "style";
    const char KEY_LANGUAGE[] = "language";


    const char KEY_GEOMETRY[] = "geometry";
    const char KEY_WINDOWSTATE[] = "windowState";

    const char KEY_SIZE[] = "size";
    const char KEY_TREESIZE[] = "treesize";
    const char KEY_TXTLOGSIZE[] = "txtlogsize";
	const char KEY_LISTTASKSIZE[] = "listtasksize";
	

    const char KEY_COMBO_FINDTEXTS[] = "combofindtexts";
    const char KEY_SHOWMISSING[] = "showmissing";

	
    const char KEY_LASTSELECTEDADDDIRECTORY[] = "lastselectedadddir";
    const char KEY_LASTSELECTEDSCANDIRECTORY[] = "lastselectedscandir";
    const char KEY_LASTSELECTEDDOCUMENT[] = "lastselecteddocumentdir";

    const char KEY_USE_CUSTOMDATABASEDIR[] = "usecustomdatabasedir";
    const char KEY_DATABASE_PATH[] = "databasepath";





    const char KEY_MAX_GETDIR_THREADCOUNT[] = "maxgetdirthreadcount";
    const char KEY_MAX_THUMBNAIL_THREADCOUNT[] = "maxthumbnailthreadcount";
    const char KEY_THUMBNAIL_COUNT[] = "thumbcount";
    const char KEY_THUMBNAIL_FORMAT[] = "thumbformat";

    const char KEY_TASK_PRIORITY[] = "taskpriority";

    const char KEY_TITLE_TEXT_TEMPLATE[] = "titletexttemplate";
    const char KEY_INFO_TEXT_TEMPLATE[] = "infotexttemplate";

    const char KEY_IMAGECACHETYPE[] = "imagecachetype";


    const char KEY_EXTENSION_ORDERALLOW[] = "extensionoderallow";
    const char KEY_ALLOW_EXTENSIONS[] = "allowextensions";
    const char KEY_DENY_EXTENSIONS[] = "denyextensions";

    const char KEY_FONT_TABLEINFO[] = "fonttableinfo";
    const char KEY_FONT_TABLEDETAIL[] = "fonttabledetail";
    const char KEY_FONT_OUPUT[] = "fontoutput";
    const char KEY_FONT_DIRECTORY[] = "fontdirectory";
    const char KEY_FONT_TASK[] = "fonttask";
    const char KEY_FONT_TAG[] = "fonttag";
    const char KEY_FONT_MENU[] = "fontmenu";
    const char KEY_FONT_STATUSBAR[] = "fontstatusbar";
    const char KEY_FONT_DOCKINGWINDOW[] = "fontdockingwindow";

	const char KEY_EXTERNALTOOLS_LASTSELECTEDEXEDIR[] = "externaltoolslastselectedexedir";

	const char KEY_EXTERNALTOOLS_COUNT[] = "externaltoolscount";
	const char KEY_EXTERNALTOOLS_GROUPPRIX[] = "externaltool_";
    const char KEY_EXTERNALTOOLS_NAME[] = "externaltoolname";
    const char KEY_EXTERNALTOOLS_EXE[] = "externaltoolexe";
    const char KEY_EXTERNALTOOLS_ARG[] = "externaltoolarg";
    const char KEY_EXTERNALTOOLS_COUNTASOPEN[] = "externaltoolcountasopen";


	const char KEY_RECENT_OPENDOCUMENTS[] = "recentdocuments";
	const char KEY_SORT[] = "sort";
	const char KEY_SORTREV[] = "sortrev";

    const char KEY_LIMIT_ITEMS[] = "limititems";
    const char KEY_LIMIT_NUMBEROFROWS[] = "limitnumberofrows";

    const char KEY_OPEN_LASTOPENEDDOCUMENT[] = "openlastdocument";

    const char KEY_FFPROBE_EXECUTABLE[] = "ffprobeexecutable";
    const char KEY_FFMPEG_EXECUTABLE[] = "ffmpegexecutable";

    const char KEY_MESSAGEBOX_REMOVEFORMEXTERNALMEDIA[] = "mb_removefromexternalmedia";


    const char FILEPART_THUMBS[] = "thumbs";

    const int THUMB_WIDTH=240;
    const int THUMB_HEIGHT=180;

    const int UUID_LENGTH=36;
    // const int THUMB_FILENAME_LENGTH=42;

    const char DEFAULT_ITEM_MAIN_TEXT[] = "${name}";
    const char DEFAULT_ITEM_SUB_TEXT[] = "${size}";

    const int MAX_COMBOFIND_SAVECOUNT = 256;

    const char STR_ENV_SCENEEXPLORER_ROOT[] = "${SCENEEXPLORER_ROOT}";

}
#endif // CONSTS_H
