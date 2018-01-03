#include "treemodel.h"

TreeModel::TreeModel()
{
    setFilter( QDir::Dirs | QDir::NoDotAndDotDot );
    setRootPath("");
}
