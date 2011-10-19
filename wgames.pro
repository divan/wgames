TEMPLATE = subdirs
SUBDIRS += src hacks

TARGET = src/wgames

TRANSLATIONS += translations/*.ts

unix {
   INSTALLS += graphics desktop icon \
        translations hacks wrapper \
	gamesini gamesimages gameshacks
   graphics.path = /usr/share/wgames/graphics
   graphics.files += data/graphics/*
   desktop.path = /usr/share/applications/hildon
   desktop.files += maemofiles/wgames.desktop
   icon.path = /usr/share/icons/hicolor/scalable/apps
   icon.files += maemofiles/wgames.png
   translations.path = /usr/share/wgames/translations
   translations.files += translations/*.qm
   gamesini.path = /usr/share/wgames/gamesinfo/ini
   gamesini.files += data/gamesinfo/ini/*.ini
   gamesimages.path = /usr/share/wgames/gamesinfo/images/
   gamesimages.files += data/gamesinfo/images/*.png
   gameshacks.path = /usr/share/wgames/gamesinfo/hacks/
   gameshacks.files += data/gamesinfo/hacks/*.sh
   hacks.path = /usr/share/wgames/lib
   hacks.files = hacks/preenv-jflatt/libsdlpre.so hacks/ldhack/libldhack.so
   wrapper.path = /usr/share/wgames
   wrapper.files = hacks/preenv-jflatt/wrapper-wgames.sh
} 
