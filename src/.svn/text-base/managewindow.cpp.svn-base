#include "managewindow.h"
#include "findipkwindow.h"
#include "installwindow.h"
#include "uninstallwindow.h"
#include "ui_managewindow.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QMenuBar>
#include <QAction>
#include <QSettings>
#include <QDir>
#include "config.h"
#include "gameslistdelegate.h"
#include "settingswindow.h"

ManageWindow::ManageWindow(QWidget *parent, GamesData &games) :
    QMainWindow(parent),
    _games(games),
    ui(new Ui::ManageWindow),
    _currentGame(0)
{
    setAttribute(Qt::WA_DeleteOnClose); // need this to catch 'destroyed' signal
    ui->setupUi(this);
    ui->runButton->hide();

    QAction * refreshAction = new QAction(tr("Refresh List"), this);
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(RefreshList()));
    QAction * settingsAction = new QAction(tr("Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(Settings()));

    QMenuBar * menuBar = new QMenuBar(this);
    menuBar->addAction(refreshAction);
    menuBar->addAction(settingsAction);

#ifdef Q_WS_MAEMO_5
    // Reimplement painter for games' list
    // to draw it in own style
    GamesListDelegate * delegate =
            new GamesListDelegate(ui->gamesList);
    ui->gamesList->setItemDelegate(delegate);
#endif

    RefreshList();
}

ManageWindow::~ManageWindow()
{
    ClearList();
    delete ui;
}

void ManageWindow::Settings()
{
    SettingsWindow * window = new SettingsWindow(this);
    window->show();
}

void ManageWindow::RefreshList()
{
    int i = (_currentGame != 0) ?
            _games.indexOf(_currentGame) :
              0;
    _games.Reload();
    ClearList();
    for (int i = 0; i < _games.count(); ++i)
    {
        const GameInfo * game = _games.at(i);
        QListWidgetItem * item = new QListWidgetItem();
        item->setData(UserRoleIndex, i);
        item->setData(UserRoleTitle, game->Title());
        item->setData(UserRoleDescription, game->Description());
        item->setData(UserRoleGenre, game->Genre());
        item->setData(UserRoleURL, game->URL());
        item->setData(UserRolePreviewImage, game->PreviewImage());
        item->setData(UserRoleStatus, game->Installed());
        item->setData(UserRoleSpace, game->SpaceRequired());
#ifndef Q_WS_MAEMO_5
        item->setText(game->Title());
#endif
        ui->gamesList->addItem(item);
    }

    // Restore current game
    _currentGame = _games.at(i);
    UpdateActionButton();
}

void ManageWindow::ClearList()
{
    ui->gamesList->blockSignals(true);
    while (ui->gamesList->count())
        delete ui->gamesList->takeItem(0);
    ui->gamesList->blockSignals(false);
}

void ManageWindow::on_gamesList_currentItemChanged( QListWidgetItem* current,
							QListWidgetItem* previous)
{
    Q_UNUSED(previous);
    _currentGame = _games.value(current->data(UserRoleIndex).toInt());

    QString info = QString(
                "<img src='%1/%2' width='%3' height='%4'><br />"
                "<span align=\"center\"><b>%5</b></span><br />"
                "%6<br />"
                "<b>WWW</b>: <a href=\"%7\">%7</a><br />"
                "<b>%8</b>: %9%10<br />"
                )
            .arg(GAMES_IMAGES_DIR)
            .arg(current->data(UserRolePreviewImage).toString())
            .arg(PREVIEW_WIDTH)
            .arg(PREVIEW_HEIGHT)
            .arg(current->data(UserRoleTitle).toString())
            .arg(current->data(UserRoleDescription).toString())
            .arg(current->data(UserRoleURL).toString())
            .arg(tr("Space Required"))
            .arg(current->data(UserRoleSpace).toString())
            .arg(tr("MB"));

    ui->descText->setText(info);
    UpdateActionButton();
}


//
// Returns true if file by mask is found
// in Downloads dir
//
bool ManageWindow::IPKFileStatus()
{
    QSettings settings(ORGANIZATION_NAME,
                       APPLICATION_NAME);

    if (!_currentGame->IPKPath().isEmpty())
        return true;

    QDir downloadsDir(
            settings.value("Main/DownloadsDir",
                   DOWNLOADS_DIR).toString());

    QStringList files = downloadsDir.entryList(
                QStringList(_currentGame->IPKFile()),
                  QDir::Files
                | QDir::NoDotAndDotDot
                | QDir::Readable,
                QDir::Name);
    bool ret(files.count() > 0);

    if (ret)
    {
        _currentGame->SetIPKPath(QString("%1/%2")
                                .arg(downloadsDir.path())
                                .arg(files.last()));
    }
    return ret;
}

void ManageWindow::on_actionButton_clicked()
{
    if (ui->actionButton->text() == tr("Find IPK file..."))
    {
        FindIpkWindow * findDlg = new FindIpkWindow(this,
                                                _currentGame);
        findDlg->exec();

        UpdateActionButton();
    }
    else if (ui->actionButton->text() == tr("Install"))
    {
        InstallGame();
    }
    else if  (ui->actionButton->text() == tr("Uninstall"))
    {
        UninstallGame();
    }
}

void ManageWindow::UpdateActionButton()
{
    if (_currentGame == 0)
        return;
    if (_currentGame->Installed())
    {
        ui->actionButton->setText(tr("Uninstall"));
        ui->ipkStatusLabel->setText("");
        ui->ipkLabel->setText("<font color='orange'>" +
                              tr("Game installed") +
                              ((!_currentGame->JsonVersion().isEmpty() ?
                                QString(tr(" (version %1)"))
                                    .arg(_currentGame->JsonVersion()) :
                                "")) +
                              "</font>");
        ui->runButton->show();

        // Don't allow Run button, if game was
        // installed with previous version
        // and binary/id info isn't stored
        ui->runButton->setEnabled(!_currentGame->JsonBinary().isEmpty() &&
                                  !_currentGame->JsonID().isEmpty());
    }
    else
    {
        bool found = IPKFileStatus();
        ui->ipkLabel->setText(tr("IPK file") + ": ");
        ui->actionButton->setText(found ?
                                  tr("Install") :
                                  tr("Find IPK file..."));
        ui->ipkStatusLabel->setText(found ? "<font color='green'>" +
                                        tr("Found!") +
                                        "</font>" :
                                        "<font color='red'>" +
                                        tr("Not Found") +
                                        "</font>");
        ui->runButton->hide();
    }
}

void ManageWindow::InstallGame()
{
    InstallWindow * window = new InstallWindow(this,
                                                _currentGame);
    connect(window, SIGNAL(destroyed()), this, SLOT(RefreshList()));
    window->show();
    window->Run();
}

void ManageWindow::UninstallGame()
{
    if (QMessageBox::question(this, tr("Uninstall game?"),
                   tr("Are you sure you want to uninstall %1?<br />"
                      "You might want to backup your saves first.")
                   .arg(_currentGame->Title()),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No)
                != QMessageBox::Yes)
        return;

    UninstallWindow * window = new UninstallWindow(this,
                                                    _currentGame);
    connect(window, SIGNAL(destroyed()), this, SLOT(RefreshList()));
    window->show();
    window->Run();
}


void ManageWindow::on_runButton_clicked()
{
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);

    qDebug() << _currentGame->Exec();

    if (!proc.startDetached(_currentGame->Exec()))
    {
        qWarning() << "Cannot launch game:<br />" <<
                      _currentGame->Exec() <<
                      " returned: <br />" <<
                      proc.readAllStandardOutput();
    }
}
