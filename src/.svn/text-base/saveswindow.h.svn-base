#ifndef SAVESWINDOW_H
#define SAVESWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "gamesdata.h"

namespace Ui {
    class SavesWindow;
}

class SavesWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SavesWindow(QWidget *parent,
                         GamesData &games);
    ~SavesWindow();

private:
    GamesData &_games;
    GameInfo *_currentGame;
    Ui::SavesWindow *ui;

    void PrintMessage(QString text);
    void PrintWarning(QString text);
    bool Restorable(GameInfo * game);

private slots:
    bool Backup(GameInfo * game);
    bool Restore(GameInfo * game);
    void BackupAll();
    void RestoreAll();
    void RefreshList();

    void on_backupButton_clicked();
    void on_restoreButton_clicked();
    void on_gamesList_currentItemChanged(
        QListWidgetItem* current,
        QListWidgetItem* previous);
};

#endif // SAVESWINDOW_H
