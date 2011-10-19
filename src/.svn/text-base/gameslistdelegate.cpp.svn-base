#include "gameslistdelegate.h"

#include <QString>
#include <QPen>
#include <QFont>
#include <QColor>
#include <QPoint>
#include <QBrush>
#include <QDate>

#include "config.h"

void GamesListDelegate::paint(QPainter *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex &index) const
{
        QString title = index.data(UserRoleTitle).toString();
        QString genre = index.data(UserRoleGenre).toString();
        bool isInstalled = index.data(UserRoleStatus).toBool();

        painter->save();
        QRect r = option.rect;
        if(option.state & QStyle::State_Selected)
        {
            r = option.rect;
#ifdef Q_WS_MAEMO_5
            painter->drawImage(r, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
#else
            painter->fillRect(r, option.palette.highlight().color());
#endif
        }
        QFont f = painter->font();
        QPen defaultPen = painter->pen();

        r = option.rect;
        f.setPointSize(18);
        painter->setFont(f);
        painter->drawText(20, r.top()+5, r.width()-15, r.height(),
                          Qt::AlignTop|Qt::AlignLeft, title, &r);

        r = option.rect;
        f.setPointSize(16);
        painter->setFont(f);
        painter->setPen(QPen(QColor("gray")));
        painter->drawText(20, r.top() + 35, r.width()-15, r.height(),
                          Qt::AlignTop|Qt::AlignLeft, genre, &r);

        if (isInstalled)
        {
            r = option.rect;
            painter->setPen(QPen(QColor("green")));
            painter->setBrush(QBrush(QColor("green")));
            painter->drawEllipse(QPoint(r.left() + 10, r.top() + 20),
                                 5, 5);
        }

        painter->restore();
}

void SavesListDelegate::paint(QPainter *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex &index) const
{
        QString title = index.data(UserRoleTitle).toString();
        QDate timestamp = index.data(UserRoleSaveTimestamp).toDate();

        painter->save();
        QRect r = option.rect;
        if(option.state & QStyle::State_Selected)
        {
            r = option.rect;
#ifdef Q_WS_MAEMO_5
            painter->drawImage(r, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
#else
            painter->fillRect(r, option.palette.highlight().color());
#endif
        }
        QFont f = painter->font();
        QPen defaultPen = painter->pen();

        r = option.rect;
        f.setPointSize(18);
        painter->setFont(f);
        painter->drawText(20, r.top()+5, r.width()-15, r.height(),
                          Qt::AlignTop|Qt::AlignLeft, title, &r);

        QString timeText(tr("Last backup: "));
        timeText += (timestamp.isNull())
                 ? tr("Never")
                 : timestamp.toString(Qt::DefaultLocaleLongDate);
        r = option.rect;
        f.setPointSize(16);
        painter->setFont(f);
        painter->setPen(QPen(QColor("gray")));
        painter->drawText(20, r.top() + 35, r.width()-15, r.height(),
                          Qt::AlignTop|Qt::AlignLeft, timeText, &r);

        painter->restore();
}

