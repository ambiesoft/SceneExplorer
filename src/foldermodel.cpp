#include "foldermodel.h"

FolderModel::FolderModel()
{
    setFilter( QDir::Dirs | QDir::NoDotAndDotDot );
    // setRootPath("");
    setReadOnly(true);
}

QVariant FolderModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::CheckStateRole)
    {
        if(checkedIndexes_.contains(index))
            return Qt::Checked;
        else if(partcheckedIndexes_.contains(index))
            return Qt::PartiallyChecked;
        else
            return Qt::Unchecked;
    }
    else if(role==Qt::DisplayRole)
    {
        return QDirModel::data(index, role);
    }
    return QDirModel::data(index, role);
}
bool FolderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::CheckStateRole)
    {
        if(value == Qt::Checked)
        {
            checkedIndexes_.insert(index);
            partcheckedIndexes_.remove(index);
            // recursiveCheck(index, value);
        }
        else if(value == Qt::PartiallyChecked)
        {
            checkedIndexes_.remove(index);
            partcheckedIndexes_.insert(index);
        }
        else
        {
            checkedIndexes_.remove(index);
            partcheckedIndexes_.remove(index);
            // recursiveCheck(index, value);
        }
        emit dataChanged(index, index);
        return true;
    }
    return QDirModel::setData(index, value, role);
}
bool FolderModel::recursiveCheck(const QModelIndex &index, const QVariant &value)
{
    if(hasChildren(index))
    {
        int i;
        int childrenCount = rowCount(index);
        QModelIndex child;
        for(i=0; i<childrenCount; i++)
        {
            child = QDirModel::index(i, 0, index);
            setData(child, value, Qt::CheckStateRole);
        }
    }
    return true;
}
