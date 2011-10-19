#ifndef GAMESLISTDELEGATE_H
#define GAMESLISTDELEGATE_H

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QModelIndex>

class GamesListDelegate : public QStyledItemDelegate
{
public:
        explicit GamesListDelegate(QObject *parent=0)
            : QStyledItemDelegate(parent) {}
        virtual ~GamesListDelegate() {}

        void paint (QPainter* painter,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) const;
};

class SavesListDelegate : public QStyledItemDelegate
{
public:
        explicit SavesListDelegate(QObject *parent=0)
            : QStyledItemDelegate(parent) {}
        virtual ~SavesListDelegate() {}

        void paint (QPainter* painter,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index) const;
};

#endif /* GAMESLISTDELEGATE_H */
