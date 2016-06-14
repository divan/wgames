#!/bin/sh

#
# Post-Install/Uninstall script for WebOS Game Skater
# Usage: ./skater.sh [install|uninstall] bin_dir data_dir
#


if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Create powervr config
    #echo -e "[default]\nForceExternalZBuffer=0\n" > ${BIN_DIR}/skater.ini

    # libavcodec hack
    #ln -s /opt/preenv/lib/libstub.so ${BIN_DIR}/libavcodec.so.52

    # Configure keyboard mapping
gconftool-2 --recursive-unset /apps/preenv/Skater
gconftool-2 --set --type int /apps/preenv/Skater/enable_dpad 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_center_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_center_x 120
gconftool-2 --set --type int /apps/preenv/Skater/dpad_center_y 357
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_n_key 119
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_n_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_n_x 146
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_n_y 271
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_ne_key 101
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_ne_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_ne_x 216
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_ne_y 300
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_e_key 100
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_e_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_e_x 233
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_e_y 347
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_se_key 99
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_se_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_se_x 205
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_se_y 409
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_s_key 120
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_s_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_s_x 150
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_s_y 427
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_sw_key 122
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_sw_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_sw_x 61
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_sw_y 411
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_w_key 97
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_w_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_w_x 60
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_w_y 347
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_nw_key 113
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_nw_mouse 1
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_nw_x 73
gconftool-2 --set --type int /apps/preenv/Skater/dpad_key_nw_y 301
gconftool-2 --set --type int /apps/preenv/Skater/key_273_key 273 # up arrow
gconftool-2 --set --type int /apps/preenv/Skater/key_273_mouse 2
gconftool-2 --set --type int /apps/preenv/Skater/key_273_x 727
gconftool-2 --set --type int /apps/preenv/Skater/key_273_y 413
gconftool-2 --set --type int /apps/preenv/Skater/key_112_key 112 # P
gconftool-2 --set --type int /apps/preenv/Skater/key_112_mouse 2
gconftool-2 --set --type int /apps/preenv/Skater/key_112_x 584
gconftool-2 --set --type int /apps/preenv/Skater/key_112_y 357
gconftool-2 --set --type int /apps/preenv/Skater/key_46_key 46 # period
gconftool-2 --set --type int /apps/preenv/Skater/key_46_mouse 2
gconftool-2 --set --type int /apps/preenv/Skater/key_46_x 584
gconftool-2 --set --type int /apps/preenv/Skater/key_46_y 357

elif [ "$1" = "uninstall" ]; then
    gconftool-2 --recursive-unset /apps/preenv/Skater
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
