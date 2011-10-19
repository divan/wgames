#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QString>

#ifdef Q_WS_MAEMO_5
    #include <QMaemo5ValueButton>
#else
    #include <QPushButton>
#endif

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

private slots:
    void on_saveButton_clicked();
    void on_restoreButton_clicked();
    void on_cancelButton_clicked();
    void downloadsDirButton_clicked();
    void savesDirButton_clicked();
    void binDirButton_clicked();
    void dataDirButton_clicked();

private:
    Ui::SettingsWindow *ui;
    QString _origDataDir;

#ifdef Q_WS_MAEMO_5
    QMaemo5ValueButton *downloadsDirButton;
    QMaemo5ValueButton *savesDirButton;
    QMaemo5ValueButton *binDirButton;
    QMaemo5ValueButton *dataDirButton;
#else
    QPushButton *downloadsDirButton;
    QPushButton *savesDirButton;
    QPushButton *binDirButton;
    QPushButton *dataDirButton;
#endif

    void LoadSettings();
    void SaveSettings();
};

#endif // SETTINGSWINDOW_H
