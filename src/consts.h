#ifndef CONSTS_H
#define CONSTS_H

namespace Consts
{
    const char ORGANIZATION[] = "Ambiesoft";
    const char APPDOMAIN[] = "ambiesoft.fam.cx";
    const char APPNAME[] = "SceneExplorer";
    const char APPNAME_DISPLAY[] = "Scene Explorer";
    const char VERSION[] = "0.1";

    const char KEY_SIZE[] = "size";
    const char KEY_TREESIZE[] = "treesize";
    const char KEY_TXTLOGSIZE[] = "txtlogsize";
	const char KEY_LISTTASKSIZE[] = "listtasksize";
	
	const char KEY_SHOWMISSING[] = "showmissing";

	
    const char KEY_LASTSELECTEDDIRECTORY[] = "lastselecteddir";
    const char KEY_DBPATH[] = "dbpath";

    const char KEY_KEY_USERENTRY_DIRECTORY_ALL_SELECTED[] = "entrydirectoryallselected";
    const char KEY_KEY_USERENTRY_DIRECTORY_ALL_CHECKED[] = "entrydirectoryallchecked";

    const char KEY_USERENTRY_DIRECTORIES[] = "entrydirectories";
    const char KEY_USERENTRY_SELECTED[] = "entryselecteds";
    const char KEY_USERENTRY_CHECKEDS[] = "entrycheckeds";

    const char KEY_MAX_GETDIR_THREADCOUNT[] = "maxgetdirthreadcount";
    const char KEY_MAX_THUMBNAIL_THREADCOUNT[] = "maxthumbnailthreadcount";

    const char ALL_TASK_FINISHED[] = "======== All Tasks finished ========";




    const char FILEPART_THUMBS[] = "thumbs";

    const int THUMB_WIDTH=240;
    const int THUMB_HEIGHT=180;

    const int UUID_LENGTH=37;
    const int THUMB_FILENAME_LENGTH=42;
}
#endif // CONSTS_H
