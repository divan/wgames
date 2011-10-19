#ifndef MANAGEWINDOW_H
#define MANAGEWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "gamesdata.h"

namespace Ui {
    class ManageWindow;
}

class ManageWindow : public QMainWindow
{
    Q_OBJECT

public:
    ManageWindow(QWidget *parent,
                          GamesData &games);
    ~ManageWindow();

private:
    GamesData &_games;
    Ui::ManageWindow *ui;
    GameInfo * _currentGame;

    bool IPKFileStatus();
    void UpdateActionButton();
    void InstallGame();
    void UninstallGame();
    void ClearList();

private slots:
    void RefreshList();
    void on_actionButton_clicked();
    void on_gamesList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void Settings();
    void on_runButton_clicked();
};

#endif // MANAGEWINDOW_H
