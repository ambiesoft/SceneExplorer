#ifndef HELPER_H
#define HELPER_H



QString rstrip(const QString& str, QChar c);
QString pathCombine(const QString& path1, const QString& path2);
QString canonicalDir(const QString& dir);
void Alert(QString message);
QString createSalient(const QString& file, const qint64& size);
void showInGraphicalShell(QWidget *parent, const QString &pathIn);
QString getUUIDFromThumbfile(const QString& file);
bool IsVideoExtention(const QString& file);


#endif // HELPER_H
