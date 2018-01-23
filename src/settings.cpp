#include <QApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QMap>
#include <QList>

#include "helper.h"
#include "settings.h"

const char* Settings::KEY_FAVORITE_COUNT = "favoritecount";
const char* Settings::KEY_FAVORITE_FOLDERS = "favoritefolders";
const char* Settings::KEY_FAVORITE_NAME = "favoritename";

const char* Settings::GROUPPREFIX_FAVORITE = "favgroup_";

void Settings::init()
{
	//    QString org = this->fileName();
	//    QFileInfo o(org);
	//    QString dbFileName = o.path() + "/" + o.completeBaseName() + ".db";

	//    db_ = QSqlDatabase::addDatabase("QSQLITE");
	//    db_.setDatabaseName(QDir::toNativeSeparators(dbFileName));
	//    if (!db_.open())
	//    {
	//        // Q_ASSERT(false);
	//        Alert(nullptr, tr("Failed to open database \"%1\". (%2)").
	//              arg(dbFileName).
	//              arg(db_.lastError().text()));
	//        QApplication::exit(1);
	//        return;
	//    }

	//    QSqlQuery query(db_);
	//    db_.exec("CREATE TABLE Favorites ("
	//               "id INTEGER PRIMARY KEY, "
	//               "nameid INTEGER, "
	//               "favid INTEGER"

	//               ")"
	//               );

	//#ifdef QT_DEBUG
	//    qDebug() << db_.lastError().text();
	//    for (int i = 0; i < db_.tables().count(); i ++) {
	//        qDebug() << db_.tables().at(i);
	//    }
	//#endif

	//    if (!query.exec("PRAGMA table_info('Favorites')"))
	//        return;

	//    while(query.next())
	//    {
	//        QString col=query.value("name").toString();
	//        allColumns_.append(col);
	//    }
	//    qDebug() << allColumns_;
	//    ok_ = true;

    int groupcount = valueInt(KEY_FAVORITE_COUNT);
    for (int i = 0; i < groupcount; ++i)
    {
        QString group = GROUPPREFIX_FAVORITE + QString::number(i);

        s_.beginGroup(group);
        QVariant vN = s_.value(KEY_FAVORITE_NAME);
        QVariant vF = s_.value(KEY_FAVORITE_FOLDERS);
        if (vN.isValid() && vF.isValid())
        {
            QString name = vN.toString();
            QStringList folders = vF.toStringList();

            // favorites_[name] = folders;
            favorites_.append(qMakePair(name,folders));
        }
        s_.endGroup();
    }

}
void Settings::AddToFavorites(const QString& name, const QStringList& dirs)
{
	//    QString state = "INSERT OR";
	//    for(const QString& dirc : lists)
	//    {
	//        QString dir = canonicalDir(dirc);
	//    }
    // favorites_[name] = dirs;
    Q_ASSERT(!this->IsNameExists(name));
    favorites_.append(qMakePair(name,dirs));
}
QStringList Settings::GetFavorites() const
{
	QStringList ret;
    FAVTYPE::const_iterator it = favorites_.constBegin();
	while (it != favorites_.constEnd())
	{
        // ret.append(it.key());
        ret.append(it->first);
		++it;
	}
	return ret;
}
QStringList Settings::GetFavorite(const QString& name) const
{
    // return favorites_[name];
    for(int i=0 ; i < favorites_.count(); ++i)
    {
        if(favorites_[i].first==name)
            return favorites_[i].second;
    }
    return QStringList();
}
Settings::~Settings()
{
    int i;
    for (i=0 ; i < favorites_.count(); ++i)
	{
        s_.beginGroup(GROUPPREFIX_FAVORITE + QString::number(i));
        s_.setValue(KEY_FAVORITE_NAME, favorites_[i].first);
        s_.setValue(KEY_FAVORITE_FOLDERS, favorites_[i].second);
        s_.endGroup();
	}
	Q_ASSERT(favorites_.count() == i);
    s_.setValue(KEY_FAVORITE_COUNT, favorites_.count());
}
int Settings::valueInt(const QString& key, int defaultvalue)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return defaultvalue;

    // setvalue(int) will be got as string
//    if(v.type()!=QVariant::Type::Int)
//        return defaultvalue;

    return v.toInt();
}
bool Settings::valueBool(const QString& key, bool defaultvalue)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return defaultvalue;

    return v.toBool();
}
QString Settings::valueString(const QString& key, const QString& defaultvalue)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return defaultvalue;
    return v.toString();
}
QStringList Settings::valueStringList(const QString& key)
{
    QVariant v = s_.value(key);
    if(!v.isValid())
        return QStringList();
    return v.toStringList();
}
QVariant Settings::value(const QString& key)
{
    return s_.value(key);
}
void Settings::setValue(const QString& key, const QVariant& v)
{
    s_.setValue(key,v);
}
