#ifndef GAMESDATA_H
#define GAMESDATA_H

#include <QList>
#include "gameinfo.h"


class GamesData : public QList<GameInfo *>
{
public:
    explicit GamesData();
    ~GamesData();

    void Reload();
    qint32 TotalCount();
    qint32 InstalledCount();
};

#endif // GAMESDATA_H
