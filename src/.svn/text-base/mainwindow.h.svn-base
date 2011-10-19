#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamesdata.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow * ui;
    GamesData * _games;

    void paintEvent(QPaintEvent*);

private slots:
    void on_savesButton_clicked();
    void on_installButton_clicked();
    void Settings();
    void UpdateLabels();
    void UpdateTracker();
};

#endif // MAINWINDOW_H
