#ifndef ACTIONWINDOW_H
#define ACTIONWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QProcess>
#include "gameinfo.h"

namespace Ui {
    class ActionWindow;
}

class ActionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ActionWindow(QWidget *parent = 0,
                          GameInfo *game = 0);
    ~ActionWindow();

    virtual void Run() {}

    Ui::ActionWindow *ui;
    GameInfo * _game;

    QMovie _movie;
    QProcess _proc1, _proc2;

    void PrintError(QString text);
    void PrintWarning(QString text);
    void PrintOK(QString text = "");
    void PrintStep(QString text);
    void PrintNote(QString text);
    void PrintDone(QString text);
    bool Failed(QString text);

    bool RemoveDir(const QString &dirName);
    bool ConfirmRemoveDir(const QString &dirName);
    void Sync();
    bool RunPostinstall(QString mode);

    bool StepUpdateStatus(bool status);

private:
    bool _finished;
    qint8 _currentStep;

    void closeEvent(QCloseEvent *event);

private slots:
    void on_returnButton_clicked();
};

#endif // ACTIONWINDOW_H
