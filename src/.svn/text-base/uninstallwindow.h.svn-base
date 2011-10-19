#ifndef UNINSTALLWINDOW_H
#define UNINSTALLWINDOW_H

#include "actionwindow.h"

class UninstallWindow : public ActionWindow
{
    Q_OBJECT

public:
    explicit UninstallWindow(QWidget *parent = 0,
                           GameInfo *game = 0);
    ~UninstallWindow();

    void Run();

private:
    bool Step1DirectoryCheck();
    bool Step2RemoveDirs();
    bool Step3RemoveShortcuts();
};

#endif // UNINSTALLWINDOW_H
