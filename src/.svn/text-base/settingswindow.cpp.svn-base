#include <QSettings>
#include <QFileDialog>

#include "settingswindow.h"
#include "ui_settingswindow.h"

#include "config.h"
#include "trackercfg.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

#ifdef Q_WS_MAEMO_5
    downloadsDirButton = new QMaemo5ValueButton(ui->scrollAreaWidgetContents);
    downloadsDirButton->setObjectName(QString::fromUtf8("downloadsDirButton"));
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(downloadsDirButton->sizePolicy().hasHeightForWidth());
    downloadsDirButton->setSizePolicy(sizePolicy);
    downloadsDirButton->setMinimumSize(QSize(0, 72));

    savesDirButton = new QMaemo5ValueButton(ui->scrollAreaWidgetContents);
    savesDirButton->setObjectName(QString::fromUtf8("savesDirButton"));
    sizePolicy.setHeightForWidth(savesDirButton->sizePolicy().hasHeightForWidth());
    savesDirButton->setSizePolicy(sizePolicy);
    savesDirButton->setMinimumSize(QSize(0, 72));

    binDirButton = new QMaemo5ValueButton(ui->scrollAreaWidgetContents);
    binDirButton->setObjectName(QString::fromUtf8("binDirButton"));
    sizePolicy.setHeightForWidth(binDirButton->sizePolicy().hasHeightForWidth());
    binDirButton->setSizePolicy(sizePolicy);
    binDirButton->setMinimumSize(QSize(0, 72));

    dataDirButton = new QMaemo5ValueButton(ui->scrollAreaWidgetContents);
    dataDirButton->setObjectName(QString::fromUtf8("dataDirButton"));
    sizePolicy.setHeightForWidth(dataDirButton->sizePolicy().hasHeightForWidth());
    dataDirButton->setSizePolicy(sizePolicy);
    dataDirButton->setMinimumSize(QSize(0, 72));

    ui->verticalLayout_2->addWidget(downloadsDirButton);
    ui->verticalLayout_2->addWidget(savesDirButton);
    ui->verticalLayout_2->addWidget(dataDirButton);
    ui->verticalLayout_2->addWidget(binDirButton);

    downloadsDirButton->setValueLayout(QMaemo5ValueButton::ValueUnderTextCentered);
    dataDirButton->setValueLayout(QMaemo5ValueButton::ValueUnderTextCentered);
    binDirButton->setValueLayout(QMaemo5ValueButton::ValueUnderTextCentered);
    savesDirButton->setValueLayout(QMaemo5ValueButton::ValueUnderTextCentered);

    connect(downloadsDirButton, SIGNAL(clicked()), this, SLOT(downloadsDirButton_clicked()));
    connect(savesDirButton, SIGNAL(clicked()), this, SLOT(savesDirButton_clicked()));
    connect(binDirButton, SIGNAL(clicked()), this, SLOT(binDirButton_clicked()));
    connect(dataDirButton, SIGNAL(clicked()), this, SLOT(dataDirButton_clicked()));
#endif

    downloadsDirButton->setText(tr("Downloads Dir"));
    dataDirButton->setText(tr("Data Dir"));
    binDirButton->setText(tr("Bin Dir"));
    savesDirButton->setText(tr("Saves Dir"));

    LoadSettings();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_saveButton_clicked()
{
    SaveSettings();
    if (dataDirButton->valueText() != _origDataDir)
    {
        TrackerCfg tracker(this, dataDirButton->valueText(),
                           _origDataDir);
        tracker.Update();
    }
    close();
}

void SettingsWindow::LoadSettings()
{
    QSettings settings(ORGANIZATION_NAME,
                       APPLICATION_NAME);

#ifdef Q_WS_MAEMO_5
    downloadsDirButton->setValueText(
                settings.value("Main/DownloadsDir",
                               DOWNLOADS_DIR).toString());
    binDirButton->setValueText(
                settings.value("Main/BinDir",
                               BIN_DIR_ROOT).toString());
    dataDirButton->setValueText(
                settings.value("Main/DataDir",
                               DATA_DIR_ROOT).toString());
    savesDirButton->setValueText(
                settings.value("Main/SavesDir",
                               SAVES_DIR_ROOT).toString());
#else
    downloadsDirButton->setText(
                downloadsDirButton->text() + "<br />" +
                settings.value("Main/DownloadsDir",
                               DOWNLOADS_DIR).toString());
    binDirButton->setText(
                binDirButton->text() + "<br />" +
                settings.value("Main/BinDir",
                               BIN_DIR_ROOT).toString());
    dataDirButton->setText(
                dataDirButton->text() + "<br />" +
                settings.value("Main/DataDir",
                               DATA_DIR_ROOT).toString());
    savesDirButton->setText(
                savesDirButton->text() + "<br />" +
                settings.value("Main/SavesDir",
                               SAVES_DIR_ROOT).toString());
#endif

    _origDataDir = settings.value("Main/DataDir",
                                  DATA_DIR_ROOT).toString();
}

void SettingsWindow::SaveSettings()
{
#ifdef Q_WS_MAEMO_5
    QSettings settings(ORGANIZATION_NAME,
                       APPLICATION_NAME);
    settings.setValue("Main/DownloadsDir",
                      downloadsDirButton->valueText());
    settings.setValue("Main/BinDir",
                      binDirButton->valueText());
    settings.setValue("Main/DataDir",
                      dataDirButton->valueText());
    settings.setValue("Main/SavesDir",
                      savesDirButton->valueText());
#endif
}

void SettingsWindow::on_restoreButton_clicked()
{
#ifdef Q_WS_MAEMO_5
    downloadsDirButton->setValueText(DOWNLOADS_DIR);
    binDirButton->setValueText(BIN_DIR_ROOT);
    dataDirButton->setValueText(DATA_DIR_ROOT);
    savesDirButton->setValueText(SAVES_DIR_ROOT);
#endif
}

void SettingsWindow::on_cancelButton_clicked()
{
    close();
}

void SettingsWindow::downloadsDirButton_clicked()
{
#ifdef Q_WS_MAEMO_5
    QString dirName = QFileDialog::getExistingDirectory(
                      this,
                      tr("Downloads Directory"),
                      "/home/user");
    if (!dirName.isEmpty())
        downloadsDirButton->setValueText(dirName);
#endif
}


void SettingsWindow::savesDirButton_clicked()
{
#ifdef Q_WS_MAEMO_5
    QString dirName = QFileDialog::getExistingDirectory(
                this,
                tr("Saves Directory"),
                "/home/user");
    if (!dirName.isEmpty())
        savesDirButton->setValueText(dirName);
#endif
}

void SettingsWindow::binDirButton_clicked()
{
#ifdef Q_WS_MAEMO_5
    QString dirName = QFileDialog::getExistingDirectory(
                      this,
                      tr("Binary Directory"),
                      "/home/user");
    if (!dirName.isEmpty())
        binDirButton->setValueText(dirName);
#endif
}

void SettingsWindow::dataDirButton_clicked()
{
#ifdef Q_WS_MAEMO_5
    QString dirName = QFileDialog::getExistingDirectory(
                      this,
                      tr("Game Data Directory"),
                      "/home/user");
    if (!dirName.isEmpty())
        dataDirButton->setValueText(dirName);
#endif
}
