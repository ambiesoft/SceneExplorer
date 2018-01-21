#include "consts.h"
#include "settings.h"

#include "externaltoolitem.h"

ExternalToolItem ExternalToolItem::Load(int i, Settings& settings)
{
    QString group = Consts::KEY_EXTERNALTOOLS_GROUPPRIX + QString::number(i);

    settings.beginGroup(group);
    QString name = settings.valueString(Consts::KEY_EXTERNALTOOLS_NAME);
    QString exe = settings.valueString(Consts::KEY_EXTERNALTOOLS_EXE);
    QString arg = settings.valueString(Consts::KEY_EXTERNALTOOLS_ARG);
    settings.endGroup();

    return ExternalToolItem(name,exe,arg);
}
void ExternalToolItem::Save(int i, Settings& settings)
{
    QString group = Consts::KEY_EXTERNALTOOLS_GROUPPRIX + QString::number(i);

    settings.beginGroup(group);
    settings.setValue(Consts::KEY_EXTERNALTOOLS_NAME, name_);
    settings.setValue(Consts::KEY_EXTERNALTOOLS_EXE, exe_);
    settings.setValue(Consts::KEY_EXTERNALTOOLS_ARG, arg_);
    settings.endGroup();

}
