#include <sys/statvfs.h>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QRegExp>

#include "ui_actionwindow.h"
#include "installwindow.h"
#include "config.h"

InstallWindow::InstallWindow(QWidget *parent,
                             GameInfo * game) :
               ActionWindow(parent, game)
{
    ui->titleLabel->setText(tr("Installing") + " <b>" +
                            _game->Title() + "</b>");
    this->setWindowTitle(tr("Install game"));
}

InstallWindow::~InstallWindow()
{
}

void InstallWindow::Run()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
    Step1SanityCheck();
}

bool InstallWindow::Step8ShowNotes()
{
    StepUpdateStatus(true);
    if (!_game->InstallNotes().isEmpty())
    {
        PrintNote(tr("Notes") + "<br />" +
                  _game->InstallNotes());
    }

    PrintDone(tr("Game installed!"));
    ui->returnButton->show();
    return true;
}


bool InstallWindow::Step7PostInstall()
{
    RunPostinstall("install");
    return Step8ShowNotes();
}

//
// Step 6
// Extract icon and create .desktop files
//
bool InstallWindow::Step6CreateShortcuts()
{
    PrintStep(tr("Prepare desktop shortcut"));

    QDir desktopDir(DESKTOP_ENTRY_PATH),
            iconDir(ICONS_PATH),
            dbusDir(DBUS_PATH);

    if (!desktopDir.exists())
    {
        if (!desktopDir.mkpath(DESKTOP_ENTRY_PATH))
            return Failed(tr("Cannot create directory %1")
                       .arg(desktopDir.path()));

    }

    if (!iconDir.exists())
    {
        if (!iconDir.mkpath(ICONS_PATH))
            return Failed(tr("Cannot create directory %1")
                       .arg(iconDir.path()));
    }
    if (!dbusDir.exists())
    {
        if (!dbusDir.mkpath(DBUS_PATH))
            return Failed(tr("Cannot create directory %1")
                       .arg(dbusDir.path()));
    }

    // Icon path
    QString fullDataDir(_game->DataPath() +
                        "/usr/palm/applications/" +
                        _game->JsonID() + "/");
    QFile iconFile(fullDataDir + _game->JsonIcon());
    QString saneName =_game->JsonTitle().toLower();
    QString newName(QString(ICONS_PATH) +
                    QString("/") +
                    saneName + ".png");
    if (iconFile.exists() && !QFile::exists(newName))
    {
            if (!iconFile.copy(newName))
            {
                return Failed(tr("Cannot copy icon ") +
                              iconFile.fileName() +
                              "to " + ICONS_PATH + "/" + saneName + ".png");
            }
    }


    QString desktopEntry  =
            "[Desktop Entry]\n"
            "Encoding=UTF-8\n"
            "Version=1.0\n"
            "Type=Application\n"
            "Name=" + _game->JsonTitle() + "\n"
            "Exec=" + _game->Exec() + "\n"
            "X-Osso-Type=application/x-executable\n"
            "X-Osso-Service=" + _game->DBusName() + "\n"
            "X-Preenv-Generated=true\n"
            "X-Preenv-Vendor=" + _game->JsonVendor() + "\n"
            "X-Maemo-Category=Games\n"
            "Icon=" + saneName + "\n";

    QFile desktopFile(QString(DESKTOP_ENTRY_PATH) + QString("/") +
                      saneName  + ".desktop");
    if (!desktopFile.open(QFile::WriteOnly | QFile::Text))
        return Failed(tr("Cannot create .desktop file ") +
                   desktopFile.fileName());
    desktopFile.write(desktopEntry.toLocal8Bit());
    desktopFile.close();

    QString dbusEntry =
            "[D-BUS Service]\n"
            "Name=" + _game->DBusName() + "\n"
            "Exec=" + _game->Exec() + "\n";

    QFile dbusFile(QString(DBUS_PATH) + QString("/")
                   + saneName  + ".service");
    if (!dbusFile.open(QFile::WriteOnly | QFile::Text))
        return Failed(tr("Cannot create DBUS .service file ") +
                      dbusFile.fileName());

    dbusFile.write(dbusEntry.toLocal8Bit());
    dbusFile.close();

    PrintOK();
    return Step7PostInstall();
}

//
// Step 5
// Copy binary to bindir, create symlinks
//
bool InstallWindow::Step5PrepareFiles()
{
    PrintStep(tr("Prepare files and symlinks"));

    QString binaryBase = _game->JsonBinary().section('/', 0, -2);
    QString binaryFilename = _game->JsonBinary().section('/', -1, -1);
    QString fullDataDir(_game->DataPath() +
                        "/usr/palm/applications/" +
                        _game->JsonID() + "/");

    // Copy binary to BinDir
    QFile bin(fullDataDir + _game->JsonBinary());
    QString toPath(_game->BinPath() + "/" + binaryFilename);
    qDebug() << "Copying binary " << bin.fileName() << " to " <<
            toPath;
    if (!bin.copy(toPath))
        return Failed(tr("Cannot copy binary file ") +
                      bin.fileName() + " to " +
                      toPath);
    QFile copiedBin(toPath);
    copiedBin.setPermissions(copiedBin.permissions() | QFile::ExeOwner | QFile::ExeGroup);

    // Create symlinks
    // Assuming main game directory is the basedir for binary
    QDir baseDir(fullDataDir + binaryBase);
    baseDir.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    QStringList filesList = baseDir.entryList();
    for (int i = 0; i < filesList.size(); ++i)
    {
        if (filesList.at(i) != binaryFilename)
        {
            QFile::link(fullDataDir + binaryBase + "/" + filesList.at(i),
                        _game->BinPath() + "/" + filesList.at(i));
            qDebug() << "Creating symlink from " <<
                    fullDataDir + binaryBase + "/" + filesList.at(i) <<
                    " to " << _game->BinPath() + "/" + filesList.at(i);
        }
    }

    PrintOK();
    Sync();

    return Step6CreateShortcuts();
}

//
// Step 4
// Unpack IPK file
//
bool InstallWindow::Step4Unpack()
{
    PrintStep(tr("Unpack archive (it may take a while)"));
    ui->animationLabel->show();
    _movie.start();

    _proc1.close();
    QStringList env = QProcess::systemEnvironment();
    env << "PATH=/usr/bin/gnu:$PATH";
    env.replaceInStrings(QRegExp("^PATH=(.*)", Qt::CaseInsensitive), "PATH=/usr/bin/gnu:$PATH");
    _proc1.setEnvironment(env);

    _proc1.start("/usr/bin/dpkg-deb -X \"" + 
                 _game->IPKPath() + "\" \"" +
		 _game->DataPath() + "\"");
    connect(&_proc1, SIGNAL(finished(int)), this, SLOT(Step4Finished()));

    return true;
}

bool InstallWindow::Step4Finished()
{
    ui->animationLabel->hide();
    _movie.stop();
    disconnect(&_proc1, SIGNAL(finished(int)), this, SLOT(Step4Finished()));

    if (_proc1.exitCode() != 0)
    {
        return Failed(tr("Cannot unpack archive:<br />") +
                      QString::fromLocal8Bit(_proc1.readAllStandardError()));
    }

    PrintOK();
    return Step5PrepareFiles();
}

//
// Step 3
// Check if directories are exists and
// create root directories if needed
//
bool InstallWindow::Step3PrepareDirs()
{
    PrintStep(tr("Prepare directories"));

    QSettings settings(ORGANIZATION_NAME,
                       APPLICATION_NAME);

    QString mainName(_game->JsonTitle());

    if (mainName.isEmpty())
        return Failed(tr("Main property not found in Json: ") +
                   mainName);

    _game->SetBinPath(settings.value("Main/BinDir",
                               BIN_DIR_ROOT).toString()  +
                       QString("/") + mainName);
    _game->SetDataPath(settings.value("Main/DataDir",
                               DATA_DIR_ROOT).toString() +
                       QString("/") + mainName);

    QDir dataDir(_game->DataPath());
    QDir binDir(_game->BinPath());

    if (dataDir.exists())
        if (!ConfirmRemoveDir(dataDir.path()))
            return false;
    if (binDir.exists())
        if (!ConfirmRemoveDir(binDir.path()))
            return false;

    if (!dataDir.mkpath(dataDir.path()))
        return Failed(QString(tr("Can't create directory %1"))
                   .arg(dataDir.path()));

    if (!binDir.mkpath(binDir.path()))
        return Failed(QString(tr("Can't create directory %1"))
                   .arg(binDir.path()));

    PrintOK();
    return Step4Unpack();
}

//
// Step 2
// Extract apping.json from .ipk file and
// check whether parameters match
//
bool InstallWindow::Step2JsonCheck()
{
    PrintStep(tr("Extracting game info"));
    ui->animationLabel->show();
    _movie.start();

    _proc1.setStandardOutputProcess(&_proc2);
    _proc1.start("/usr/bin/dpkg-deb --fsys-tarfile \"" + _game->IPKPath() + "\"");
    _proc2.start(TAR_APP" x --wildcards \"*appinfo.json\" -O");
    // TODO: find if we can wait until first bytes in output and
    // stop process
    connect(&_proc2, SIGNAL(finished(int)), this, SLOT(Step2Finished()));
    return true;
}

bool InstallWindow::Step2Finished()
{
    ui->animationLabel->hide();
    _movie.stop();
    disconnect(&_proc2, SIGNAL(finished(int)), this, SLOT(Step2Finished()));

    if (_proc2.exitCode() != 0)
        return Failed(tr("Cannot parse JSON: ") +
                   QString::fromLocal8Bit(_proc2.readAllStandardError()));

    QString result = QString::fromLocal8Bit(_proc2.readAllStandardOutput());

    // Parse returned Json answer
    if (!_game->ParseJson(result))
        return Failed(tr("Error parsing appinfo.json!"));

    // Check if ID matches
    QRegExp rx(_game->ID());
    if (!_game->JsonID().contains(rx))
        return Failed(tr("Seems to be wrong game or naming error!<br />"
                         "Ipk ID: %1<br />"
                         "Expected ID mask: %2")
                      .arg(_game->JsonID())
                      .arg(_game->ID()));

    PrintOK(_game->JsonTitle() + " " +
            _game->JsonVersion() + tr(" by ") +
            _game->JsonVendor());

    return Step3PrepareDirs();
}

//
// Step 1
// Check free disk space
// TODO: check for unusual partition tables
//
bool InstallWindow::Step1SanityCheck()
{
    PrintStep(tr("Checking available space"));

    ui->stepsLabel->repaint();
    unsigned long long result(0);
    struct statvfs sfs;
    if (::statvfs(MYDOCS_DIR, &sfs) != -1)
        result = (unsigned long long)sfs.f_bsize * sfs.f_bfree;

    qint32 freeSpace = result/(1024*1024);

    if (_game->SpaceRequired() >= freeSpace)
        return Failed(QString(tr("Available free space in /home/user/MyDocs directory "
                   "is %1MB, but the game requires %2MB!"))
                      .arg(freeSpace)
                      .arg(_game->SpaceRequired()));

    PrintOK();

    return Step2JsonCheck();
}
