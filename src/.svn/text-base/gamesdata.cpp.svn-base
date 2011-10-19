#include "gamesdata.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QSettings>

#include "config.h"

GamesData::GamesData()
    : QList<GameInfo *>()
{
    Reload();
}

GamesData::~GamesData()
{
    while (!isEmpty())
         delete takeFirst();
}

void GamesData::Reload()
{
    while (!isEmpty())
         delete takeFirst();

    QDir gamesDir(GAMES_DATA_DIR);
    if (!gamesDir.exists())
    {
        qWarning() << "Games Dir doesn't exist! (" <<
                      GAMES_DATA_DIR << ")";
        return;
    }

    QStringList filter("*.ini");
    QStringList fileList = gamesDir.entryList(filter, QDir::Files);
    QString str;
    foreach (str, fileList)
    {
        GameInfo * game = new GameInfo(GAMES_DATA_DIR"/" + str);
        append(game);
    }
}

qint32 GamesData::TotalCount()
{
    return count();
}

qint32 GamesData::InstalledCount()
{
    qint32 ret(0);
    for (int i = 0; i < count(); ++i)
        ret += (at(i)->Installed()) ? 1 : 0;
    return ret;
}
