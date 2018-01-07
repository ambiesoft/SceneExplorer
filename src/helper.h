#ifndef HELPER_H
#define HELPER_H

#include <QString>

QString rstrip(const QString& str, QChar c);
QString pathCombine(const QString& path1, const QString& path2);
void Alert(QString message);
QString createSalient(const QString& file, const qint64& size);
#endif // HELPER_H
