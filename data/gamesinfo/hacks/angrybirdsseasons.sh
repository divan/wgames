#!/bin/sh

#
# Post-Install/Uninstall script for WebOS Game Angry Birds Seasons
# Usage: ./angrybirdsseasons.sh [install|uninstall] bin_dir data_dir
#


if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Create powervr config
    echo -e "[default]\nForceExternalZBuffer=0\n" > ${BIN_DIR}/angrybirdsseasons.ini
    # Configure keyboard mapping
      gconftool-2 --recursive-unset /apps/preenv/angrybirdsseasons
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/enable_dpad 0
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_119_key 119 # W left screec
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_119_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_119_x 120
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_119_y 300
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_274_key 274 # down arrow
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_274_mouse 2
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_274_x 675
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_274_y 445
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_120_key 120 # x  left screen
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_120_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_120_x 120
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_120_y 411
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_273_key 273 # up arrow
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_273_mouse 2
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_273_x 738
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_273_y 318
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_97_key 97 # A
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_97_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_97_x 60
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_97_y 357
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_100_key 100 # D
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_100_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_100_x 180
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_100_y 358

      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_101_key 101 # E
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_101_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_101_x 178
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_101_y 302

      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_113_key 113 # q
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_113_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_113_x 60
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_113_y 302

      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_99_key 99 # C
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_99_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_99_x 180
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_99_y 410

      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_122_key 122 # Z
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_122_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_122_x 61
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_122_y 411

      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_112_key 112 # P
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_112_mouse 1
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_112_x 738
      gconftool-2 --set --type int /apps/preenv/angrybirdsseasons/key_112_y 318

elif [ "$1" = "uninstall" ]; then
    true
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0