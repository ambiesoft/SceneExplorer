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
    QList<qint64> alltaggedtagids_;
    QList<qint64> tagids_;
    mutable QList<qint64> removedTagids_;
    void CopyMember(const TagidsInfo& that)
    {
        this->infotype_ = that.infotype_;
        this->alltaggedtagids_=that.alltaggedtagids_;
        this->tagids_=that.tagids_;
    }
public:
    TagidsInfo(){}
    TagidsInfo(const TagidsInfo& that){
        CopyMember(that);
    }
    ~TagidsInfo(){}

    TagidsInfo(TAGIDS_INFO_TYPE t) : infotype_(t){}

    bool operator ==(const TagidsInfo& that) {
        return
                this->infotype_ == that.infotype_ &&
                this->tagids_== that.tagids_ &&
                this->alltaggedtagids_ == that.alltaggedtagids_;
    }
    const TagidsInfo& operator=(const TagidsInfo& that) {
        if(this != &that)
        {
            CopyMember(that);
        }
        return *this;
    }
    bool isAll() const {
        return infotype_ == TAGIDSINFO_ALL;
    }
    void setAll() {
        infotype_ = TAGIDSINFO_ALL;
    }

    bool isNotags() const {
        return infotype_ == TAGIDSINFO_NOTAGS;
    }
    void setNotags() {
        infotype_ = TAGIDSINFO_NOTAGS;
    }
    void setTagIds(QList<qint64>& tagids) {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS || infotype_ == TAGIDSINFO_USERSELECTED);
        tagids_=tagids;
    }
    int tagCount() const {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS || infotype_ == TAGIDSINFO_USERSELECTED);
        return tagids_.count();
    }
    qint64 tagid(int i) const {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS || infotype_ == TAGIDSINFO_USERSELECTED);
        return tagids_[i];
    }

    void setAllTaggedTagids(QList<qint64>& alltaggedtagids) {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS);
        alltaggedtagids_ = alltaggedtagids;
    }
    int alltaggedtagidCount() const {
        Q_ASSERT(infotype_ == TAGIDSINFO_NOTAGS);
        return alltaggedtagids_.count();
    }
    qint64 alltaggedtagid(int i) const {
        return alltaggedtagids_[i];
    }


    int removedTaggedCount() const;
    qint64 removedTag(int i) const;

private:
    mutable bool bRemovedTagConstructed = false;
    void constructRemoveTaggedCount() const;
};

#endif // TAGIDSINFO_H
