#ifndef INSTALLWINDOW_H
#define INSTALLWINDOW_H

#include "actionwindow.h"

class InstallWindow : public ActionWindow 
{
    Q_OBJECT

public:
    explicit InstallWindow(QWidget *parent = 0,
                           GameInfo *game = 0);
    ~InstallWindow();

    void Run();

private:
    // Install steps
    bool Step1SanityCheck();
    bool Step2JsonCheck();
    bool Step3PrepareDirs();
    bool Step4Unpack();
    bool Step5PrepareFiles();
    bool Step6CreateShortcuts();
    bool Step7PostInstall();
    bool Step8ShowNotes();

private slots:
    bool Step2Finished();
    bool Step4Finished();
};

#endif // INSTALLWINDOW_H
