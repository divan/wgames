#include "actionwindow.h"
#include "ui_actionwindow.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QCloseEvent>
#include <QMessageBox>
#include <QScrollBar>
#ifdef Q_WS_MAEMO_5
    #include <QMaemo5InformationBox>
#endif

#include "config.h"

ActionWindow::ActionWindow(QWidget *parent,
                                   GameInfo *game) :
    QMainWindow(parent),
    ui(new Ui::ActionWindow),
    _game(game),
    _finished(false),
    _currentStep(1)
{
    ui->setupUi(this);

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
    setWindowFlags(Qt::Window);
#endif
    setAttribute(Qt::WA_DeleteOnClose); // need this to catch 'destroyed' signal

    _movie.setFileName(GRAPHICS_DIR"animation.gif");
    ui->animationLabel->setMovie(&_movie);
    ui->animationLabel->hide();
    ui->returnButton->setText(tr("Return"));
    ui->returnButton->hide();

    ui->stepsLabel->setText("<b><u>" + tr("Progress") + "</u></b>");
}

ActionWindow::~ActionWindow()
{
    delete ui;
}

void ActionWindow::closeEvent(QCloseEvent *event)
{
    if (!_finished && QMessageBox::question(this, tr("Abort installation?"),
                   tr("Are you sure you want to abort installation?"),
                   QMessageBox::Yes | QMessageBox::No,
                   QMessageBox::No)
        != QMessageBox::Yes)
    {
        event->ignore();
        return;
    }

    if (_proc1.state() != QProcess::NotRunning)
    {
        _proc1.close();
        _proc1.waitForFinished();
    }
    if (_proc2.state() != QProcess::NotRunning)
    {
        _proc2.close();
        _proc2.waitForFinished();
    }
}

//
// Print red error message
//
void ActionWindow::PrintError(QString text)
{
    ui->stepsLabel->setText(ui->stepsLabel->text() + "<br />" +
                            "<font color='red'>" +
                            tr("Failed!") +
                            "<br />" + text +
                             "</font>");
    qWarning() << "WARNING: " + text;
}

//
// Print orange warning message
//
void ActionWindow::PrintWarning(QString text)
{
    ui->stepsLabel->setText(ui->stepsLabel->text() + "<br />" +
                            "<font color='orange'>" +
                            tr("Warning: ") +
                            "<br />" + text +
                             "</font>");
}

//
// Print green success message
//
void ActionWindow::PrintDone(QString text)
{
    ui->stepsLabel->setText(ui->stepsLabel->text() + "<br />" +
                            "<font color='green'><b>" +
                            text +
                             "<b></font>");
}

//
// Pring green OK text and (optionally) grey text in
// brackets
//
void ActionWindow::PrintOK(QString text)
{
    QString new_text;
    new_text = ui->stepsLabel->text() +
               "<font color='green'>OK</font>";
    if (!text.isEmpty())
        new_text +=  " <font color='grey'>(" +
                    text +
                    ")</font>";
    ui->stepsLabel->setText(new_text);
}

//
// Print Step N text and increase step counter.
//
void ActionWindow::PrintStep(QString text)
{
    ui->stepsLabel->setText(ui->stepsLabel->text() + "<br />" +
                            QString("<b>" + tr("Step %1") + ":</b> ")
                            .arg(_currentStep) +
                            text + "... ");
    //ui->centralwidget->repaint();
    _currentStep++;
}

//
// Print Notes text
//
void ActionWindow::PrintNote(QString text)
{
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this, text,
                                           QMaemo5InformationBox::NoTimeout);
#else
        QMessageBox::information(this,
                                 "Notes",
                                 text,
                                 QMessageBox::Ok);
#endif
}

//
// Remove non-empty directory and all content
//
bool ActionWindow::RemoveDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName))
    {
        Q_FOREACH(QFileInfo info,
                  dir.entryInfoList(QDir::NoDotAndDotDot |
                                    QDir::System |
                                    QDir::Hidden  |
                                    QDir::AllDirs |
                                    QDir::Files,
                                    QDir::DirsFirst))
        {
            if (info.isDir())
            {
                result = RemoveDir(info.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result)
            {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

bool ActionWindow::ConfirmRemoveDir(const QString &dirName)
{
    if (QMessageBox::question(this, tr("Remove dir?"),
                              tr("Directory %1 already exists, perhaps from "
                                 "the previous installation.<br />"
                                 "Do you want to delete it and continue installation?")
        .arg(dirName),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No)
        != QMessageBox::Yes)

        return Failed(tr("Directory %1 already exists.<br />"
                   "Please, delete it manually"
                   " and repeat an installation."));


    RemoveDir(dirName);
    return true;
}

void ActionWindow::on_returnButton_clicked()
{
    close();
}

bool ActionWindow::Failed(QString text)
{
    PrintError(text);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
    _finished = true;
    ui->returnButton->show();
    return false;
}

bool ActionWindow::StepUpdateStatus(bool status)
{   
    PrintStep(tr("Updating status"));

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
    _game->SetInstalled(status);
    _game->SaveStatus();
    _finished = true;
    PrintOK();
    return true;
}


void ActionWindow::Sync()
{
    QProcess syncProc;
    syncProc.start("/bin/sync");
    syncProc.waitForFinished();
}

bool ActionWindow::RunPostinstall(QString mode)
{
    if (_game->PostInstall().isEmpty())
        return true;

    PrintStep(tr("Running postinstall"));

    QString script(GAMES_HACKS_DIR +
                   _game->PostInstall() +
                   " " + mode +
                   " \"" + _game->BinPath() + "\"" +
                   " \"" + _game->DataPath() + "\"");
    qDebug() << "Running script: /bin/sh "
             << script;

    QProcess proc;
    proc.start("/bin/sh " + script);
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.waitForFinished(120000);

    if (proc.exitCode() != 0)
    {
         PrintWarning(tr("Postinstall script failed!<br />"
                               "Please, check the output:<br />")
                            + proc.readAllStandardOutput());
         return true;
    }

    PrintOK();
    return true;
}
