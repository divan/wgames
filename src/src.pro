QT       += core gui maemo5 script

TARGET = wgames
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    managewindow.cpp \
    gamesdata.cpp \
    gameinfo.cpp \
    findipkwindow.cpp \
    actionwindow.cpp \
    installwindow.cpp \
    uninstallwindow.cpp \
    gameslistdelegate.cpp \
    settingswindow.cpp \
    saveswindow.cpp \
    trackercfg.cpp

HEADERS  += config.h \
    mainwindow.h \
    managewindow.h \
    gamesdata.h \
    gameinfo.h \
    findipkwindow.h \
    actionwindow.h \
    installwindow.h \
    uninstallwindow.h \
    gameslistdelegate.h \
    settingswindow.h \
    saveswindow.h \
    trackercfg.h

FORMS    += ui/mainwindow.ui \
    ui/managewindow.ui \
    ui/findipkwindow.ui \
    ui/actionwindow.ui \
    ui/settingswindow.ui \
    ui/saveswindow.ui

unix:!symbian {
    maemo5 {
        target.path = /usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}
