#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QDate>

#ifdef Q_WS_MAEMO_5
    #include <QMaemo5InformationBox>
#endif

#include "saveswindow.h"
#include "gameslistdelegate.h"
#include "ui_saveswindow.h"

#include "config.h"

SavesWindow::SavesWindow(QWidget *parent,
                         GamesData &games) :
    QMainWindow(parent),
    _games(games),
    ui(new Ui::SavesWindow)
{
    setAttribute(Qt::WA_DeleteOnClose); // need this to catch 'destroyed' signal
    ui->setupUi(this);

    QAction * backupAllAction = new QAction(tr("Backup All"), this);
    connect(backupAllAction, SIGNAL(triggered()), this, SLOT(BackupAll()));
    QAction * restoreAllAction = new QAction(tr("Restore All"), this);
    connect(restoreAllAction, SIGNAL(triggered()), this, SLOT(RestoreAll()));
    ui->menubar->addAction(backupAllAction);
    ui->menubar->addAction(restoreAllAction);

    // Reimplement painter for games' list
    // to draw it in own style
    SavesListDelegate * delegate =
            new SavesListDelegate(ui->gamesList);
    ui->gamesList->setItemDelegate(delegate);

    RefreshList();
}

SavesWindow::~SavesWindow()
{
    delete ui;
}

void SavesWindow::RefreshList()
{
    _games.Reload();

    for (int i = 0;
         i < _games.TotalCount();
         ++i)
    {
        GameInfo * game = _games.value(i);
        if (game->Installed())
        {
            QListWidgetItem * item = new QListWidgetItem();
            item->setText(game->Title());
            item->setData(UserRoleIndex, i);
            item->setData(UserRoleTitle, game->Title());
            item->setData(UserRoleSaveTimestamp, game->GetSaveTimestamp());
            ui->gamesList->addItem(item);
        }
    }
}

bool SavesWindow::Backup(GameInfo * game)
{
    if (game->SaveFiles().isEmpty())
        return true;

    qDebug() << "Backing up saves for game "
             << game->JsonTitle()
             << game->SaveFiles();

    QString dirName(SAVES_DIR_ROOT"/" + game->JsonTitle());
    QDir dir(dirName);
    if (!dir.exists())
    {
        if (!dir.mkpath(dirName))
        {
            PrintWarning(tr("Can't create dir %1")
                                     .arg(dirName));
            return false;
        }
    }

    QString str;
    foreach (str, game->SaveFiles())
    {
        QString fileName = QString("%1/%2")
                .arg(game->BinPath())
                .arg(str);
        QString copyName(QString("%1/%2")
                         .arg(dirName)
                         .arg(str));
        QFile file(fileName);

        // Try to delete destination file first
        // as QFile::copy can't overwrite
        QFile dest(copyName);
        if (dest.exists())
            dest.remove();

        // Create absolute dir with subdirs
        // if not exists
        QFileInfo info(copyName);
        QDir baseDir = info.absoluteDir();
        if (!baseDir.exists())
            baseDir.mkpath(baseDir.path());

        qDebug() << "Copying " << fileName
                 << " to " << copyName;
        // Actual copy
        if (!file.copy(copyName))
        {
            PrintWarning(QString(tr("Can't copy file %1 to %2"))
                                     .arg(fileName)
                                     .arg(copyName));
            return false;
        }
    }

    _currentGame->SetSaveTimestamp(QDate::currentDate());
    _currentGame->SaveStatus();

    return true;
}

bool SavesWindow::Restore(GameInfo * game)
{
    if (!Restorable(game))
        return true;

    qDebug() << "Restoring saves for game " << game->JsonTitle();

    QString dirName(SAVES_DIR_ROOT"/" + game->JsonTitle());
    QDir dir(dirName);
    if (!dir.exists())
    {
        PrintWarning(tr("Can't find the directory %1<br />"
                        "Backup was created, but"
                        "probably deleted manually.")
                                 .arg(dirName));
        return false;
    }

    QString str;
    foreach (str, game->SaveFiles())
    {
        QString saveName(QString("%1/%2")
                         .arg(dirName)
                         .arg(str));
        QString restoreName = QString("%1/%2")
                .arg(game->BinPath())
                .arg(str);
        QFile file(saveName);
        if (!file.exists())
        {
            PrintWarning(
             tr("Can't find the file %1<br />"
                "Backup was created, but"
                "probably file was deleted manually.")
            .arg(saveName));
            break;
        }

        qDebug() << "Saving " << saveName
                 << " to " << restoreName;
        QFileInfo fileInfo(restoreName);
        if (fileInfo.isSymLink())
        {
            qDebug() << "Resolving symlink "
                     << restoreName
                     << " -> "
                     << fileInfo.symLinkTarget();
            restoreName = fileInfo.symLinkTarget();
        }

        // Try to delete destination file first
        // as QFile::copy can't overwrite
        QFile dest(restoreName);
        if (dest.exists())
            dest.remove();

        // Actual copy
        if (!file.copy(restoreName))
        {
            PrintWarning(tr("Can't copy file %1 to %2<br />"
                            "Backup was created, but"
                            "probably file was deleted manually.")
                         .arg(saveName)
                         .arg(restoreName));
            break;
        }
    }

    return true;
}

void SavesWindow::BackupAll()
{
    for (int i = 0;
         i < ui->gamesList->count();
         ++i)
    {
        int index = ui->gamesList->
                item(i)->data(UserRoleIndex)
                .toInt();
        GameInfo * game = _games.value(index);
        Backup(game);
    }
    PrintMessage(tr("Games information backed up"));
}

void SavesWindow::RestoreAll()
{
    for (int i = 0;
         i < ui->gamesList->count();
         ++i)
    {
        int index = ui->gamesList->
                item(i)->data(UserRoleIndex)
                .toInt();
        GameInfo * game = _games.value(index);
        Restore(game);
    }
    PrintMessage(tr("Games information restored"));
}

void SavesWindow::on_gamesList_currentItemChanged(
    QListWidgetItem* current,
    QListWidgetItem* previous)
{
    Q_UNUSED(previous);
    _currentGame = _games.value(
                current->data(UserRoleIndex).toInt());

    ui->backupButton->setEnabled(!_currentGame->SaveFiles().isEmpty());
    ui->restoreButton->setEnabled(Restorable(_currentGame));
}

void SavesWindow::on_backupButton_clicked()
{
    if (Backup(_currentGame))
        PrintMessage(tr("Game information is saved"));
}

void SavesWindow::on_restoreButton_clicked()
{
    if (Restore(_currentGame))
        PrintMessage(tr("Game information restored"));
}

void SavesWindow::PrintMessage(QString text)
{
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this,
                                           "<br /><br />" +
                                           text +
                                           "<br /><br />",
                                           QMaemo5InformationBox::NoTimeout);
#else
        QMessageBox::information(this,
                                 tr("Done!"),
                                 text,
                                 QMessageBox::Ok);
#endif
}

//
// Returns true if game has files to restore
//
bool SavesWindow::Restorable(GameInfo * game)
{
    return (!game->SaveFiles().isEmpty() &&
            !game->GetSaveTimestamp().isNull());
}

void SavesWindow::PrintWarning(QString text)
{
    qWarning() << "Warning: " << text;
    QMessageBox::warning(this,
                         tr("Warning"),
                         text);
}
