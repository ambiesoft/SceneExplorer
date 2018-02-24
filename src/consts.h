#ifndef CONSTS_H
#define CONSTS_H

namespace Consts
{
    const char ORGANIZATION[] = "Ambiesoft";
    const char APPDOMAIN[] = "ambiesoft.fam.cx";
    const char APPNAME[] = "SceneExplorer";
    const char APPNAME_DISPLAY[] = "Scene Explorer";
    const char VERSION[] = "0.2";

    const char KEY_SIZE[] = "size";
    const char KEY_TREESIZE[] = "treesize";
    const char KEY_TXTLOGSIZE[] = "txtlogsize";
	const char KEY_LISTTASKSIZE[] = "listtasksize";
	
	const char KEY_SHOWMISSING[] = "showmissing";

	
    const char KEY_LASTSELECTEDDIRECTORY[] = "lastselecteddir";

    const char KEY_USE_CUSTOMDATABASEDIR[] = "usecustomdatabasedir";
    const char KEY_DATABASE_PATH[] = "databasepath";





    const char KEY_MAX_GETDIR_THREADCOUNT[] = "maxgetdirthreadcount";
    const char KEY_MAX_THUMBNAIL_THREADCOUNT[] = "maxthumbnailthreadcount";
    const char KEY_THUMBNAIL_COUNT[] = "thumbcount";

    const char KEY_IMAGECACHETYPE[] = "imagecachetype";


    const char KEY_EXTENSION_ORDERALLOW[] = "extensionoderallow";
    const char KEY_ALLOW_EXTENSIONS[] = "allowextensions";
    const char KEY_DENY_EXTENSIONS[] = "denyextensions";

    const char KEY_FONT_TABLEINFO[] = "fonttableinfo";
    const char KEY_FONT_TABLEDETAIL[] = "fonttabledetail";
    const char KEY_FONT_OUPUT[] = "fontoutput";

    const char KEY_EXTERNALTOOLS_COUNT[] = "externaltoolscount";
    const char KEY_EXTERNALTOOLS_GROUPPRIX[] = "externaltool_";
    const char KEY_EXTERNALTOOLS_NAME[] = "externaltoolname";
    const char KEY_EXTERNALTOOLS_EXE[] = "externaltoolexe";
    const char KEY_EXTERNALTOOLS_ARG[] = "externaltoolarg";


    const char KEY_RECENT_OPENDOCUMENTS[] = "recentdocuments";
    const char KEY_OPEN_LASTOPENEDDOCUMENT[] = "openlastdocument";

    const char KEY_FFPROBE_EXECUTABLE[] = "ffprobeexecutable";
    const char KEY_FFMPEG_EXECUTABLE[] = "ffmpegexecutable";


    const char ALL_TASK_FINISHED[] = "======== All Tasks finished ========";
    const char FILEPART_THUMBS[] = "thumbs";

    const int THUMB_WIDTH=240;
    const int THUMB_HEIGHT=180;

    const int UUID_LENGTH=37;
    const int THUMB_FILENAME_LENGTH=42;
}
#endif // CONSTS_H
