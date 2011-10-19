#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "ui_actionwindow.h"
#include "uninstallwindow.h"
#include "config.h"

UninstallWindow::UninstallWindow(QWidget *parent,
                             GameInfo * game) :
               ActionWindow(parent, game)
{
    ui->titleLabel->setText(tr("Uninstalling") + " <b>" +
                            _game->Title() + "</b>");
    this->setWindowTitle(tr("Uninstall game"));
}

UninstallWindow::~UninstallWindow()
{
}

void UninstallWindow::Run()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
    Step1DirectoryCheck();
}

//
// Uninstall Step 3
// Remove shortcut, icon and dbus service
//
bool UninstallWindow::Step3RemoveShortcuts()
{
    PrintStep(tr("Cleaning up"));
    QFile iconFile(QString(ICONS_PATH) + QString("/") +
                               _game->JsonTitle().toLower() + ".png");
    QFile desktopFile(QString(DESKTOP_ENTRY_PATH) + QString("/") +
                      _game->JsonTitle().toLower()  + ".desktop");
    QFile dbusFile(QString(DBUS_PATH) + QString("/")
                   + _game->JsonTitle().toLower()  + ".service");

    qDebug() << "Removing file " + iconFile.fileName();
    if (!iconFile.remove())
    {
       PrintWarning(QString(tr("Cannot delete file %1"))
                  .arg(iconFile.fileName()));
    }
    qDebug() << "Removing file " + desktopFile.fileName();
    if (!desktopFile.remove())
    {
       PrintWarning(QString(tr("Cannot delete file %1"))
                  .arg(desktopFile.fileName()));
    }
    qDebug() << "Removing file" + dbusFile.fileName();
    if (!dbusFile.remove())
    {
       PrintWarning(QString(tr("Cannot delete file %1"))
                  .arg(dbusFile.fileName()));
    }

    PrintOK();
    RunPostinstall("uninstall");
    StepUpdateStatus(false);
    PrintDone(tr("Game uninstalled successfully!"));

    ui->returnButton->show();

    return true;
}

//
// Uninstall Step 2
// Remove directories
//
bool UninstallWindow::Step2RemoveDirs()
{
    PrintStep(tr("Removing game"));

    qDebug() << "Removing dir " + _game->DataPath();
    if (!RemoveDir(_game->DataPath()))
    {
        PrintWarning(QString(tr("Cannot delete dir %1"))
                   .arg(_game->DataPath()));
    }
    qDebug() << "Removing dir " + _game->BinPath();
    if (!RemoveDir(_game->BinPath()))
    {
        PrintWarning(QString(tr("Cannot delete dir %1"))
                   .arg(_game->BinPath()));
    }

    PrintOK();
    return Step3RemoveShortcuts();
}

//
// Uninstall Step 1
// Check for directories existance
//
bool UninstallWindow::Step1DirectoryCheck()
{
    PrintStep(tr("Checking directories"));

    if (_game->JsonTitle().isEmpty())
        return Failed(tr("Game Title is not found. "
                   "That means some manual edit or "
                   "program bug."));


    QDir dataDir(_game->DataPath());
    QDir binDir(_game->BinPath());

    if (!dataDir.exists())
    {
        PrintWarning(QString(tr("Data directory %1 does not exist.<br />"
                   "This means some manual intervention "
                   "or the program bug."))
                .arg(_game->DataPath()));
    }
    if (!binDir.exists())
    {
        PrintWarning(QString(tr("Bin directory %1 does not exist.<br />"
                   "This means some manual intervention "
                   "or the program bug."))
                .arg(_game->BinPath()));
    }

    PrintOK();
    return Step2RemoveDirs();
}
