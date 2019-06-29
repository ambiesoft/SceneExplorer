# SceneExplorer

SceneExplore explores video files through thumbnails.

## Feature
* You can explorers video files by viewing its thumbnails.
* Supported video format is dependent on FFmpeg.

## Download
Download the binary from <https://github.com/ambiesoft/SceneExplorer/releases>.

## License
This software is freeware. See LICENSE.

## Environment
Windows7 or above and Linux, maybe MacOS too.

## Install
Installation is not required, extract an archive file.


## FFmpeg and FFprobe
SceneExplorer needs FFmpeg and FFprove to create thumbnails. In windows distribution, these files are included in the archive file, you do not need to obtain them by yourself. 


## How to use
### Add a folder containing videos.
Choose **[Folder]->[Add Folder]**.


### Creating thumbnails
* From the folder pane, Right-click a folder and select **[Rescan to create thumbnails]**.
* Or from the menu, select **[Task]->[start scan to create thumbnails...]** to start creating thumbnails.

### View thumbnails
From the folder pane, choose a folder to show its thumbnails. Clicking **[All]** shows all thumbnails and **[Missing]** shows thumbnails which original video files are missing.

### Adding Tags
Tags provide a useful way to group related videos together and tell what a video is about. Tags also make it easier to find your content. Tags are similar to but more specific than, directories. The use of tags is completely optional.

You can create a new tag from **[Tag]->[Add New Tag...]** and it will be shown on Tag pane. If you select Tag, tagged videoes will be shown.

## Files and Directories
There are 3 types of path SceneExplorer uses.
### Database directory
The image files of thumbnails are stored under DatabaseDirectory/thumbs/ and its information is stored in database DatabaseDirectory/db.sqlite3. DatabaseDirectory can be configured by Preference Option or by command-line option "-d". Default location is *C:\Users\<Username>\AppData\Local\Ambiesoft\SceneExplorer*.

### Preference file
This file stores app configurations modified by a user through Option Dialogs. You can change the location by using FolderConfig.exe". Default location is *C:\Users\<Username>\AppData\Roaming\Ambiesoft\SceneExplorer\SceneExplorer.ini*.

### Document file
This file holds Folders which user assigns. If you supply filename in command-line, the file will open. Default location is *C:\Users\<Username>\Documents\SceneExplorer\default.scexd*.


## Portable
If you want to make SceneExplorer portable, follow these instructions.
1. Launch **FolderConfig.exe** and select "Under this folder".
2. Launch SceneExplorer.exe, open **option**, Check **Use custom database directory**, and enter **Database directory** as *${SCENEEXPLORER_ROOT}/db*.
3. Or use command-line opton "-d" to specifiy the database directory.
4. Create a document file in this directory.
5. Create a *.bat file in the directory and write down the script as follow. This example launch **MyApp.scexd**.
```
start "" %~dp0SceneExplorer.exe %~dp0MyApp.scexd
```
6. Or create a shortcut file.

See *Help -> About Documents* to confirm which files are used.

## Uninstall
1. Select **[Help] -> [About Document]** and find which directory is used. 
2. Remove those files and directories. 


## Build
### Windows (by QtCreator)
Open 'src\SceneExplorer.pro' and build.

### Windows (incomplete)
Get the source and build tools by running the following command.
```
git clone https://github.com/ambiesoft/distSolution.git
git clone https://github.com/ambiesoft/profile.git
git clone https://github.com/ambiesoft/lsMisc.git
git clone https://github.com/ambiesoft/SceneExplorer.git
```

Copy *prepare.bat.sample* (in the directory *SceneExplorer*) and rename it to *prepare.bat* and edit it to set correct Environment values like as follows.
```
set PYTHONEXE=C:\local\python3.5\python.exe
set QTROOT=C:\local\Qt

set SOURCEDIR=src
set PRONAME=SceneExplorer

set FFMPEGSOURCEDIR=C:\LegacyPrograms\ffmpeg
set FFCEXE=C:\LegacyPrograms\FFC\FFC.exe
set DISTDIR=C:\Linkout\SceneExplorer
```

Run *build.bat*. This will build SceneExplorer into *C:\Linkout\SceneExplorer*.

### Linux
Get the source and build tools by running the following command.
```
git clone https://github.com/ambiesoft/profile.git
git clone https://github.com/ambiesoft/lsMisc.git
git clone https://github.com/ambiesoft/SceneExplorer.git
```

Build SceneExplorer with following commands.
```
$ cd SceneExplorer
$ mkdir build
$ cd build
$ qmake ../src/SceneExplorer.pro
$ make
```

Run
```
./SceneExplorer
```




## Support
If you have troubles, post *Issue* on <https://github.com/ambiesoft/SceneExplorer/issues>.

## Donate
Support Ambiesoft by making a donation. See <http://ambiesoft.fam.cx/donate/>.

## Contact
- Author: Ambiesoft trueff
- E-mail: <ambiesoft.trueff@gmail.com>
- Webpage: <http://ambiesoft.fam.cx/main/index.php?page=sceneexplorer>
- Forum: <http://ambiesoft.fam.cx/minibbs/minibbs.php>
- Development: <https://github.com/ambiesoft/SceneExplorer>


## Legal
### FFmpeg
This software uses code of [FFmpeg](http://ffmpeg.org) licensed under the 
[GPLv3](https://www.gnu.org/licenses/gpl-3.0.html) and its 
source can be downloaded [here](https://github.com/ambiesoft/FFmpeg).

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
* Icons made by <a href="https://www.flaticon.com/authors/pixelmeetup" title="Pixelmeetup">Pixelmeetup</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a>
