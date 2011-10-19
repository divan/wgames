#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QString>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString localeName = QLocale::system().name();

    QTranslator translator;
    translator.load("wgames_" + localeName,
                    BASE_DIR"/translations/");
    a.installTranslator(&translator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + localeName,
                 QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
