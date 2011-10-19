#!/bin/sh

#
# Post-Install/Uninstall script for WebOS Game Angry Birds
# Usage: ./angrybirds.sh [install|uninstall] bin_dir data_dir
#


if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Create powervr config
    echo -e "[default]\nForceExternalZBuffer=0\n" > ${BIN_DIR}/angrybirdsrio.ini

    gconftool-2 --recursive-unset /apps/preenv/angrybirdsrio
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/enable_dpad 0

    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_109_key 109 # M - menu 
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_109_mouse 1
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_109_x 58
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_109_y 42

    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_113_key 113 # Q - left area 
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_113_mouse 1
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_113_x 48
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_113_y 240

    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_112_key 112 # P - right area 
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_112_mouse 2
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_112_x 751
    gconftool-2 --set --type int /apps/preenv/angrybirdsrio/key_112_y 240

elif [ "$1" = "uninstall" ]; then
    gconftool-2 --recursive-unset /apps/preenv/angrybirdsrio
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
