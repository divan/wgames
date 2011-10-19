#ifndef FINDIPKWINDOW_H
#define FINDIPKWINDOW_H

#include <QDialog>
#include "gameinfo.h"

namespace Ui {
    class FindIpkWindow;
}

class FindIpkWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FindIpkWindow(QWidget *parent = 0, GameInfo * game = 0);
    ~FindIpkWindow();

private:
    Ui::FindIpkWindow *ui;
    GameInfo * _game;

private slots:
    void on_locateButton_clicked();
};

#endif // FINDIPKWINDOW_H
