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

#ifndef SQL_H
#define SQL_H

#include <QSqlDatabase>

#include "tableitemdata.h"
#include "globals.h"
#include "tagidsinfo.h"

#define DBFILENAME "./db.sqlite3"
#define DBVERSION 2

class LimitArg
{
    bool enabled_;
    int index_;
    int num_;
public:
    LimitArg() : enabled_(false) {}
    LimitArg(int index,int num) : enabled_(true), index_(index), num_(num) {}

    operator bool() const {
        return enabled_;
    }
    int GetOffset() const {
        return index_ * num_;
    }
    int GetCount() const {
        return num_;
    }
};

class Sql : public QObject
{
    Q_OBJECT

    QSqlDatabase db_;
    QString dbid_;
    bool ok_=false;
    QString docFile_;
    QString docdb_ = "nodb";
    QStringList allColumns_;
    int GetMovieFileInfo(const QString& movieFile,
                         bool& exist,
                         qint64& size,
                         QString& directory,
                         QString& name,
                         QString* salient,
                         qint64& ctime,
                         qint64& wtime) const;

    int RemoveEntryFromThumbID(const QString& thumbid);
    QStringList getAllColumnNames();
    QString getAllColumns(bool bBrace, bool bQ);
    // QString getAllColumnsUpdate(TableItemDataPointer tid);
    QString lastError_;

    bool GetAllSqlString(
            QSqlQuery& query,
            const QStringList& selects,
            const QStringList& dirs,
            const QString& find,
            SORTCOLUMNMY sortcolumn,
            bool sortrev,
            const LimitArg& limit,
            const TagidsInfo& tagInfos);
    bool CreateDBInfoTable();
    int GetDBVersion();
    bool UpdateDatabase1_2();
public:
    enum SQL_ERROR {
        NO_ERROR,
        MOVIEFILE_NOT_FOUND,
        FILESIZE_UNDERZERO,
        ERROR_CREATE_SALIENT,
        THUMBFILE_NOT_FOUND,
        UUID_FORMAT_ERROR,
        SQL_PREPARE_FAILED,
        SQL_EXEC_FAILED,
        THUMB_EXIST,
        THUMB_NOT_EXIST,
        THUMBID_IS_NOT_UUID,
    };
    bool isOK() const {
        return ok_;
    }
    QString getDbID() const {
        return dbid_;
    }
    static QString getDBFileName() {
        return DBFILENAME;
    }
    QString getLastError() const {
        return lastError_;
    }
    static QString getErrorStrig(int thumbRet);

    QSqlQuery* pQDeleteFromDirectoryName_ = nullptr;
    QSqlQuery* getDeleteFromDirectoryName();

    QSqlQuery* pQInsert_=nullptr;
    QSqlQuery* getInsertQuery(TableItemDataPointer tid);

    QSqlQuery* pQGetInfo_=nullptr;
    QSqlQuery* getGetInfoQuery();

    Sql();
    ~Sql();
    qint64 AppendData(TableItemDataPointer tid);
    bool IsSameFile(const QString& dir,
                    const QString& name,
                    const qint64& size,
                    const QString& salient);
    int filterWithEntry(const QString& movieDir,
                        const QStringList& movieFiles,
                        QStringList& results);
    int hasThumb(const QString& movieFile) ;
    bool DeleteEntryThumbFiles(const QString& dir,
                               const QString& name,
                               QString* removedThumbID);
    qlonglong GetAllCount(const QStringList& dirs);
    bool GetAll(QList<TableItemDataPointer>& v,
                const QStringList& dirs = QStringList(),
                const QString& find = QString(),
                bool bOnlyMissing = false,
                SORTCOLUMNMY sortcolumn = SORT_NONE,
                bool sortrev = false,
                const LimitArg& limit = LimitArg(),
                const TagidsInfo& tagInfos = TagidsInfo()
            );

    // bool IncrementOpenCount(const QString& movieFile);

    int GetAllEntry(const QString& dir,
                    QStringList& entries,
                    QVector<qint64>& sizes,
                    QVector<qint64>& ctimes,
                    QVector<qint64>& wtimes,
                    QStringList& salients);
    bool RenameEntries(const QString& dir,
                       const QStringList& renameOlds,
                       const QStringList& renameNews);
    bool UpdateThumbExtFromFile_obsolete(const qint64& id,const QString& thumbid, QString& ext);
    bool IsEntryExists(const QString& newDir, const QString& newFile);
    bool RenameEntry(const QString& dbDir,
                     const QString& dbFile,
                     const QString& newdir,
                     const QString& newfile);
    bool getEntryFromSalient(const QString& salient,
                             QStringList& dirsDB,
                             QStringList& filesDB,
                             QList<qint64>& sizesDB);

    bool hasEntry(const QString& dir,
                  const QString& file,

                  const qint64& size,
                  const qint64& wtime,

                  const QString& sa);
    bool RemoveEntry(const QString& dir,
                     const QString& file,
                     QString* error = nullptr);

    bool RemoveAllMissingEntries(const QString& dir);
    //    bool ApplyOpenCount(const QMap<qint64,int>& opencounts);
    //    bool ApplyLastAccesses(const QMap<qint64,qint64>& lastaccesses);

    bool AttachDocument(const QString& docFile);
    bool DetachDocument();

    QString lastError() const {
        return lastError_;
    }
    QString docdb() const {
        Q_ASSERT(!docdb_.isEmpty() && docdb_ != "nodb");
        return docdb_;
    }

    bool GetID(const QString& dir,
               const QString& file,
               qint64& id);
};

#endif // SQL_H
