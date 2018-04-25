# SceneExplorer
SceneExploreは動画ファイルのサムネイルを表示するアプリケーションです。

## 特徴
* サムネイルから動画を探せます。
* サポートされる動画フォーマットはFFmpegに依存します。


## ダウンロード
ダウンロードはこちらか行えます。<https://github.com/ambiesoft/SceneExplorer/releases>.


## ライセンス
このソフトウェアはフリーウェアです。LICENSEを御覧ください。


## 動作環境
Windows7以上で動作します。LinuxやmacOSでも動作するかもしれません。


## インストール
インストールは必要ありません。アーカイブを解凍してください。


## FFmpeg と FFprobe
SceneExplorerはサムネイルを作成するためにFFmpegとFFproveが必要になります。Windowsの配布ファイルにはこれらが含まれています。これらはLGPL互換モードでビルドされたものなのでいくつかの機能が存在しない可能性がありサムネイルを作成できない可能性があります。オプション設定で実行ファイル指定できるので変えたい場合は変えてください。


## 使い方
### フォルダを追加する
メニューから**[フォルダ]→[フォルダの追加]**を選択します。

### サムネイルを作成
* フォルダペインからフォルダを右クリックし、 **[スキャンしてサムネイルを作成]**を選択して作成できます。
* メニューから**[タスク]→[スキャンしてサムネイルを作成する]**を選択して作成できます。

### サムネイルを表示
フォルダペインからフォルダを選択するとそのフォルダのサムネイルが表示されます。[すべて]を選択するとすべてのサムネイルが表示されます。[存在しない]を選択するとファイルがなくなったサムネイルが表示されます。


## ファイルやディレクトリ
このアプリが使う3つのタイプのパスがあります。
### データベースディレクトリ
サムネイルの画像ファイルとその管理のデータベースを置くディレクトリです。設定やコマンドライン(-d)から変えられます。デフォルトの位置は*C:\Users<Username>\AppData\Local\Ambiesoft\SceneExplorer*です。

### 設定ファイル
アプリの設定を保存するファイルです。*FolderConfig.exe*ツールで変更できます。デフォルトの位置は*C:\Users<Username>\AppData\Roaming\Ambiesoft\SceneExplorer\SceneExplorer.ini*です。

### ドキュメントファイル
アプリで追加したフォルダが保存されます。コマンドラインのメイン引数で指定できます。デフォルトの位置は*C:\Users<Username>\Documents\SceneExplorer\default.scexd*です。


## ポータブル
ポータブルで使いたいときは以下の手順で行います。
1. **FolderConfig.exe**を起動して、**exeのあるフォルダ**を選択します。
2. **SceneExplorer.exe**を起動して、オプションを開き、**カスタムデータベースディレクトリを使う**をチェックし、**データベースディレクトリ**に *${SCENEEXPLORER_ROOT}/db* と入力します。
3. このフォルダ内にドキュメントファイルをつくります。

実際の有効な設定値は*ヘルプ ⇒ ドキュメントについて*を参照してください。


## ビルド
### Windows
以下のコマンドでソースとビルドツールを取得します。
```
git clone https://github.com/ambiesoft/distSolution
git clone https://github.com/ambiesoft/SceneExplorer
```

*prepare.bat.sample* をコピーしてからリネームして、*prepare.bat*に変えます。その後このファイルを編集して自分の環境に合わせます。例として以下のようになります。
```
set PYTHONEXE=C:\local\python3.5\python.exe
set QTROOT=C:\local\Qt

set SOURCEDIR=src
set PRONAME=SceneExplorer

set FFMPEGSOURCEDIR=C:\LegacyPrograms\ffmpeg
set FFCEXE=C:\LegacyPrograms\FFC\FFC.exe
set DISTDIR=C:\Linkout\SceneExplorer
```

build.batを実行します。*C:\Linkout\SceneExplorer* にビルドされます。


## サポート
If you have troubles, post *Issue* on <https://github.com/ambiesoft/SceneExplorer/issues>.

## コンタクト
- 作者: Ambiesoft trueff
- E-mail: <ambiesoft.trueff@gmail.com>
- ウェブページ: <http://ambiesoft.fam.cx/main/index.php?page=sceneexplorer>
- 掲示板: <http://ambiesoft.fam.cx/minibbs/minibbs.php>
- 開発: <https://github.com/ambiesoft/SceneExplorer>


## 法的情報
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
