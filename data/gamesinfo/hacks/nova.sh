#!/bin/sh

#
# Post-Install/Uninstall script for WebOS Game N.O.V.A
# Usage: ./nova.sh [install|uninstall] bin_dir data_dir
#


if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Configure keyboard mapping
    gconftool-2 --recursive-unset /apps/preenv/nova
    gconftool-2 --set --type int /apps/preenv/nova/enable_dpad 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_center_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_center_x 120
    gconftool-2 --set --type int /apps/preenv/nova/dpad_center_y 357
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_n_key 119
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_n_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_n_x 120
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_n_y 300
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_ne_key 101
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_ne_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_ne_x 178
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_ne_y 302
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_e_key 100
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_e_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_e_x 180
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_e_y 358
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_se_key 99
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_se_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_se_x 180
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_se_y 410
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_s_key 120
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_s_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_s_x 120
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_s_y 411
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_sw_key 122
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_sw_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_sw_x 61
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_sw_y 411
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_w_key 97
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_w_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_w_x 60
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_w_y 357
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_nw_key 113
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_nw_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_nw_x 62
    gconftool-2 --set --type int /apps/preenv/nova/dpad_key_nw_y 302

    gconftool-2 --set --type int /apps/preenv/nova/key_109_key 109 # M - menu
    gconftool-2 --set --type int /apps/preenv/nova/key_109_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/key_109_x 32
    gconftool-2 --set --type int /apps/preenv/nova/key_109_y 26
    gconftool-2 --set --type int /apps/preenv/nova/key_107_key 107 # K - power kick
    gconftool-2 --set --type int /apps/preenv/nova/key_107_mouse 1
    gconftool-2 --set --type int /apps/preenv/nova/key_107_x 60
    gconftool-2 --set --type int /apps/preenv/nova/key_107_y 95

    gconftool-2 --set --type int /apps/preenv/nova/key_32_key 32 # Space - jump
    gconftool-2 --set --type int /apps/preenv/nova/key_32_mouse 2
    gconftool-2 --set --type int /apps/preenv/nova/key_32_x 780
    gconftool-2 --set --type int /apps/preenv/nova/key_32_y 460
    gconftool-2 --set --type int /apps/preenv/nova/key_108_key 108 # L - fire
    gconftool-2 --set --type int /apps/preenv/nova/key_108_mouse 2
    gconftool-2 --set --type int /apps/preenv/nova/key_108_x 701
    gconftool-2 --set --type int /apps/preenv/nova/key_108_y 384
    gconftool-2 --set --type int /apps/preenv/nova/key_105_key 105 # I - grenades
    gconftool-2 --set --type int /apps/preenv/nova/key_105_mouse 2
    gconftool-2 --set --type int /apps/preenv/nova/key_105_x 720
    gconftool-2 --set --type int /apps/preenv/nova/key_105_y 250
    gconftool-2 --set --type int /apps/preenv/nova/key_111_key 111 # O - reload
    gconftool-2 --set --type int /apps/preenv/nova/key_111_mouse 2
    gconftool-2 --set --type int /apps/preenv/nova/key_111_x 715
    gconftool-2 --set --type int /apps/preenv/nova/key_111_y 84
elif [ "$1" = "uninstall" ]; then
    gconftool-2 --recursive-unset /apps/preenv/nova
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
# Навигатор по играм WebOS: http://maemoworld.ru/navigator.php?catalog=webos_games

