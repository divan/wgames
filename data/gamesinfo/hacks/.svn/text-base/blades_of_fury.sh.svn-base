#!/bin/sh

#
# Post-Install/Uninstall script for WebOS Blades Of Fury 
# Usage: ./blades_of_fury.sh [install|uninstall] bin_dir data_dir
#


if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Configure keyboard mapping
    gconftool-2 --recursive-unset /apps/preenv/Blades
    gconftool-2 --set --type int /apps/preenv/Blades/enable_dpad 0
    gconftool-2 --set --type int /apps/preenv/Blades/key_119_key 119
    gconftool-2 --set --type int /apps/preenv/Blades/key_119_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_119_x 120
    gconftool-2 --set --type int /apps/preenv/Blades/key_119_y 300
    gconftool-2 --set --type int /apps/preenv/Blades/key_101_key 101
    gconftool-2 --set --type int /apps/preenv/Blades/key_101_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_101_x 178
    gconftool-2 --set --type int /apps/preenv/Blades/key_101_y 302
    gconftool-2 --set --type int /apps/preenv/Blades/key_100_key 100
    gconftool-2 --set --type int /apps/preenv/Blades/key_100_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_100_x 180
    gconftool-2 --set --type int /apps/preenv/Blades/key_100_y 358
    gconftool-2 --set --type int /apps/preenv/Blades/key_99_key 99
    gconftool-2 --set --type int /apps/preenv/Blades/key_99_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_99_x 180
    gconftool-2 --set --type int /apps/preenv/Blades/key_99_y 410
    gconftool-2 --set --type int /apps/preenv/Blades/key_120_key 120
    gconftool-2 --set --type int /apps/preenv/Blades/key_120_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_120_x 120
    gconftool-2 --set --type int /apps/preenv/Blades/key_120_y 411
    gconftool-2 --set --type int /apps/preenv/Blades/key_122_key 122
    gconftool-2 --set --type int /apps/preenv/Blades/key_122_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_122_x 61
    gconftool-2 --set --type int /apps/preenv/Blades/key_122_y 411
    gconftool-2 --set --type int /apps/preenv/Blades/key_97_key 97
    gconftool-2 --set --type int /apps/preenv/Blades/key_97_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_97_x 60
    gconftool-2 --set --type int /apps/preenv/Blades/key_97_y 357
    gconftool-2 --set --type int /apps/preenv/Blades/key_113_key 113
    gconftool-2 --set --type int /apps/preenv/Blades/key_113_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_113_x 60
    gconftool-2 --set --type int /apps/preenv/Blades/key_113_y 302

    gconftool-2 --set --type int /apps/preenv/Blades/key_109_key 109
    gconftool-2 --set --type int /apps/preenv/Blades/key_109_mouse 1
    gconftool-2 --set --type int /apps/preenv/Blades/key_109_x 18
    gconftool-2 --set --type int /apps/preenv/Blades/key_109_y 18

    gconftool-2 --set --type int /apps/preenv/Blades/key_112_key 112
    gconftool-2 --set --type int /apps/preenv/Blades/key_112_mouse 2
    gconftool-2 --set --type int /apps/preenv/Blades/key_112_x 757
    gconftool-2 --set --type int /apps/preenv/Blades/key_112_y 269
    gconftool-2 --set --type int /apps/preenv/Blades/key_111_key 111
    gconftool-2 --set --type int /apps/preenv/Blades/key_111_mouse 2
    gconftool-2 --set --type int /apps/preenv/Blades/key_111_x 736
    gconftool-2 --set --type int /apps/preenv/Blades/key_111_y 354
    gconftool-2 --set --type int /apps/preenv/Blades/key_107_key 107
    gconftool-2 --set --type int /apps/preenv/Blades/key_107_mouse 2
    gconftool-2 --set --type int /apps/preenv/Blades/key_107_x 655
    gconftool-2 --set --type int /apps/preenv/Blades/key_107_y 427
    gconftool-2 --set --type int /apps/preenv/Blades/key_108_key 108
    gconftool-2 --set --type int /apps/preenv/Blades/key_108_mouse 2
    gconftool-2 --set --type int /apps/preenv/Blades/key_108_x 756
    gconftool-2 --set --type int /apps/preenv/Blades/key_108_y 439
elif [ "$1" = "uninstall" ]; then
    gconftool-2 --recursive-unset /apps/preenv/Blades
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
