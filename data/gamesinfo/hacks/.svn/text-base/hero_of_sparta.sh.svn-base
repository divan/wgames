
#!/bin/sh

#
# Driver Post-Install/Uninstall script for WebOS Game Hero Of Sparta
# Usage: ./hero_of_sparta.sh [install|uninstall] bin_dir data_dir
#

if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    #
    # Some install actions
    #
    gconftool-2 --recursive-unset /apps/preenv/HeroOfSparta
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/enable_dpad 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_center_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_center_x 120
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_center_y 357
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_n_key 119
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_n_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_n_x 120
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_n_y 300
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_ne_key 101
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_ne_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_ne_x 178
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_ne_y 302
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_e_key 100
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_e_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_e_x 180
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_e_y 358
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_se_key 99
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_se_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_se_x 180
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_se_y 410
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_s_key 120
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_s_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_s_x 120
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_s_y 411
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_sw_key 122
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_sw_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_sw_x 61
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_sw_y 411
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_w_key 97
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_w_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_w_x 60
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_w_y 357
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_nw_key 113
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_nw_mouse 1
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_nw_x 62
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/dpad_key_nw_y 302

    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_111_key 111
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_111_mouse 2
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_111_x 600
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_111_y 423
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_112_key 112
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_112_mouse 2
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_112_x 723
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_112_y 357
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_46_key 46
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_46_mouse 2
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_46_x 724
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_46_y 226
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_109_key 109
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_109_mouse 2
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_109_x 40
    gconftool-2 --set --type int /apps/preenv/HeroOfSparta/key_109_y 113
elif [ "$1" = "uninstall" ]; then
    #
    # Some uninstall actions
    # 
    gconftool-2 --recursive-unset /apps/preenv/HeroOfSparta
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
