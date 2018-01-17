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

	QVariant v = this->value(KEY_FAVORITE_COUNT).toInt();
	if (v.isValid())
	{
		int groupcount = v.toInt();
		for (int i = 0; i < groupcount; ++i)
		{
			QString group = GROUPPREFIX_FAVORITE + QString::number(i);

			beginGroup(group);
			QVariant vN = this->value(KEY_FAVORITE_NAME);
			QVariant vF = this->value(KEY_FAVORITE_FOLDERS);
			if (vN.isValid() && vF.isValid())
			{
				QString name = vN.toString();
				QStringList folders = vF.toStringList();

				favorites_[name] = folders;

			}
		}
	}
}
void Settings::AddToFavorites(const QString& name, const QStringList& dirs)
{
	//    QString state = "INSERT OR";
	//    for(const QString& dirc : lists)
	//    {
	//        QString dir = canonicalDir(dirc);
	//    }
	favorites_[name] = dirs;
}
QStringList Settings::GetFavorites() const
{
	QStringList ret;
	QMap<QString, QStringList>::const_iterator it = favorites_.constBegin();
	while (it != favorites_.constEnd())
	{
		ret.append(it.key());
		++it;
	}
	return ret;
}
QStringList Settings::GetFavorite(const QString& name) const
{
	return favorites_[name];
}
Settings::~Settings()
{
	QMap<QString, QStringList>::const_iterator it = favorites_.constBegin();
	int i = 0;
	for (;  it != favorites_.constEnd() ; ++it, ++i)
	{
		beginGroup(GROUPPREFIX_FAVORITE + QString::number(i));
		setValue(KEY_FAVORITE_NAME, it.key());
		setValue(KEY_FAVORITE_FOLDERS, it.value());
		endGroup();
	}
	Q_ASSERT(favorites_.count() == i);
	setValue(KEY_FAVORITE_COUNT, favorites_.count());
}
