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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QSettings>
#include <QList>

struct DE
{
    QString dir_;
    bool chk_;
    bool sel_;

    DE(const QString& dir, bool chk, bool sel) :
        dir_(dir),
        chk_(chk),
        sel_(sel){}

};

class Document : public QObject
{
    QSettings* s_ = nullptr;

    bool bAllSel_ = false;
    bool bAllChecked_ = false;

    QList<DE> delist_;

    QString lastErr_;

    QString file_;

public:
    Document(){}
    ~Document() {
        delete s_;
    }
    bool Load(const QString& file, const bool bExists);

    QString GetLastErr() const {
        return lastErr_;
    }
    void Store(QListWidget* pLW);

    bool IsAllSelected() const {
        return bAllSel_;
    }
    bool IsAllChecked() const {
        return bAllChecked_;
    }

    int dirCount() const {
        return delist_.count();
    }
    QString GetDEText(int i) const {
        return delist_[i].dir_;
    }
    bool IsDESelected(int i) const {
        return delist_[i].sel_;
    }
    bool IsDEChecked(int i) const {
        return delist_[i].chk_;
    }

    QString GetFileName() const;
    QString GetFullName() const;
};

#endif // DOCUMENT_H
