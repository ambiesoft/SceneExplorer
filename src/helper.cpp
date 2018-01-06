#include <QDir>
#include <QMessageBox>

#include "helper.h"

QString rstrip(const QString& str, QChar c) {
  int n = str.size() - 1;
  for (; n >= 0; --n) {
    if (str.at(n) != c) {
      return str.left(n + 1);
    }
  }
  return "";
}

QString pathCombine(const QString& path1, const QString& path2)
{
    if(QDir::isAbsolutePath(path2))
        return path2;


    return rstrip(path1, '/') + '/' + rstrip(path2, '/');
}

void Alert(QString message)
{
    QMessageBox msgBox;
    msgBox.setText("Name");
    msgBox.setInformativeText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
