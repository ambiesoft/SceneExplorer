# SceneExplorer

SceneExplore explores video files by viewing its thumbnails.

## FFmpeg and FFprobe
SceneExplorer needs FFmpeg and FFprove to create thumbnails. In windows distribution, as these files are included in the archive file, you do not need to obtain them by yourself. But these files are built by LGPL compatible mode, some feature needed to create particular thumbnails is missing. You can configure SceneExplore to use custom FFmpeg and FFprobe from Option.

## Add a directory to filter thumbnails

## Creating thumbnails


## Buid
### Windows
Get the source and build tools by running following command.
```
git clone https://github.com/ambiesoft/distSolution
git clone https://github.com/ambiesoft/SceneExplorer
```

Rename *qtroot.bat.sample* (in directory *SceneExplorer*) to *qtroot.bat* and edit it to set correct Qt install directory as follows.
```
set QTROOT=C:\local\Qt
```

Rename *pythonexe.bat.sample* to *pythonexe.bat* and edit it to set correct python executable as follows.
```
set PYTHONEXE=C:\local\python3.5\python.exe
```

Run build.bat. This will build SceneExplorer into *C:\Linkout\SceneExplorer*.

## Add
This software uses code of <a href=http://ffmpeg.org>FFmpeg</a> licensed under the 
<a href=https://www.gnu.org/licenses/gpl-3.0.html>GPLv3</a> and its 
source can be downloaded <a href=https://github.com/ambiesoft/SceneExplorer>here</a>.
