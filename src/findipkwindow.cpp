#include "findipkwindow.h"
#include "ui_findipkwindow.h"
#include <QFileDialog>

#include "config.h"

FindIpkWindow::FindIpkWindow(QWidget *parent, GameInfo * game) :
    QDialog(parent),
    ui(new Ui::FindIpkWindow),
    _game(game)
{
    ui->setupUi(this);
    ui->mainLabel->setText(tr("Manager can't find file <b>%1</b> in <b>MyDocs/Downloads</b> "
			    "folder. Download it into this folder or locate "
			    "in the different folder.<br />"
			    "<font color=\"red\">Note, that there is <b>NO OFFICIAL WAY</b> to buy PalmPre games. "
			    "The only possible way is to search google and download "
			    "it by yourself.</font>")
			   .arg(_game->IPKFile()));
}

FindIpkWindow::~FindIpkWindow()
{
    delete ui;
}

void FindIpkWindow::on_locateButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Locate IPK File"),
         MYDOCS_DIR,
         tr("IPK (*.ipk)"));
    if (!fileName.isEmpty())
        _game->SetIPKPath(fileName);
    done(0);
    return;
}
