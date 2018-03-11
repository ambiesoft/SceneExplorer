# SceneExplorer

SceneExplore explores video files by viewing its thumbnails.

## Feature
* You can explorers video files by viewing its thumbnails.
* Supported video format is dependent on FFmpeg.

## Download
Download the binary from <https://github.com/ambiesoft/SceneExplorer/releases>.

## Environment
Windows7 or above and Linux, maybe MacOS too.

## Install
Installation is not required, extract an arhive file.


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

Copy and rename *prepare.bat.sample* (in directory *SceneExplorer*) to *prepare.bat* and edit it to set correct Environment values like as follows.
```
set PYTHONEXE=C:\local\python3.5\python.exe
set QTROOT=C:\local\Qt

set SOURCEDIR=src
set PRONAME=SceneExplorer

set FFMPEGSOURCEDIR=C:\LegacyPrograms\ffmpeg
set FFCEXE=C:\LegacyPrograms\FFC\FFC.exe
set DISTDIR=C:\Linkout\SceneExplorer
```


Run build.bat. This will build SceneExplorer into *C:\Linkout\SceneExplorer*.


## Support
If you have troubles, post *Issue* on <https://github.com/ambiesoft/SceneExplorer/issues>.

## Contact
- Author: Ambiesoft trueff
- E-mail: <ambiesoft.trueff@gmail.com>
- Webpage: <http://ambiesoft.fam.cx/main/index.php?page=sceneexplorer>
- Forum: <http://ambiesoft.fam.cx/minibbs/minibbs.php>
- Development: <https://github.com/ambiesoft/SceneExplorer>


## Legal
### FFmpeg
This software uses code of <a href=http://ffmpeg.org>FFmpeg</a> licensed under the 
<a href=https://www.gnu.org/licenses/gpl-3.0.html>GPLv3</a> and its 
source can be downloaded <a href=https://github.com/ambiesoft/SceneExplorer>here</a>.

### Qt
This software uses Qt.
Source code: <https://github.com/ambiesoft/qt5/tree/5.10>

### Icons
* Icons made by <a href="http://www.freepik.com" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
* Icons made by <a href="https://www.flaticon.com/authors/kiranshastry" title="Kiranshastry">Kiranshastry</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
* Icons made by <a href="https://www.flaticon.com/authors/gregor-cresnar" title="Gregor Cresnar">Gregor Cresnar</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
* Icons made by <a href="https://www.flaticon.com/authors/dave-gandy" title="Dave Gandy">Dave Gandy</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
* Icons made by <a href="http://www.freepik.com" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
* Icons made by <a href="https://www.flaticon.com/authors/smashicons" title="Smashicons">Smashicons</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
