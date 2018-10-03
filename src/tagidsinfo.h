#ifndef TAGIDSINFO_H
#define TAGIDSINFO_H

#include <QSet>
#include <QList>
class TagidsInfo
{
public:
    enum TAGIDS_INFO_TYPE{
        TAGIDSINFO_NONE,
        TAGIDSINFO_ALL,
        TAGIDSINFO_NOTAGS,
        TAGIDSINFO_USERSELECTED,
    };
private:
    TAGIDS_INFO_TYPE infotype_ = TAGIDSINFO_ALL;
    QSet<qint64> fileids_;
public:
    TagidsInfo(){}
    TagidsInfo(TAGIDS_INFO_TYPE t) : infotype_(t){}

    bool operator ==(const TagidsInfo& that) {
        return
                this->infotype_ == that.infotype_ &&
                this->fileids_== that.fileids_;
    }
    const TagidsInfo& operator=(const TagidsInfo& that) {
        if(this != &that)
        {
            this->infotype_ = that.infotype_;
            this->fileids_ = that.fileids_;
        }
        return *this;
    }
    bool isAll() const {
        return infotype_ == TAGIDSINFO_ALL;
    }
    void setAll() {
        infotype_ = TAGIDSINFO_ALL;
        fileids_.clear();
    }

    bool isNotags() const {
        return infotype_ == TAGIDSINFO_NOTAGS;
    }
    void setNotags() {
        infotype_ = TAGIDSINFO_NOTAGS;
        fileids_.clear();
    }
    bool isFileIdEmpty() const {
        Q_ASSERT(infotype_ == TAGIDSINFO_USERSELECTED);
        return fileids_.empty();
    }
    int fileIdCount() const {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS || infotype_ == TAGIDSINFO_USERSELECTED);
        return fileids_.count();
    }
    void setFileIds(QSet<qint64>& fileids) {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS || infotype_ == TAGIDSINFO_USERSELECTED);
        fileids_ = fileids;
    }
    const QSet<qint64>& fileIds() const {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS || infotype_ == TAGIDSINFO_USERSELECTED);
        return fileids_;
    }
};

#endif // TAGIDSINFO_H
