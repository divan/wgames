
#!/bin/sh

#
# Driver Post-Install/Uninstall script for WebOS Game 
# Usage: ./driver.sh [install|uninstall] bin_dir data_dir
#

if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Create powervr config
    echo -e "[default]\nForceExternalZBuffer=0\n" > ${BIN_DIR}/driver.ini

    #
    # Some install actions
    #
        gconftool-2 --recursive-unset /apps/preenv/driver
        gconftool-2 --set --type int /apps/preenv/driver/enable_dpad 0

        gconftool-2 --set --type int /apps/preenv/driver/key_109_key 109 
        gconftool-2 --set --type int /apps/preenv/driver/key_109_mouse 2
        gconftool-2 --set --type int /apps/preenv/driver/key_109_x 750
        gconftool-2 --set --type int /apps/preenv/driver/key_109_y 36

        gconftool-2 --set --type int /apps/preenv/driver/key_113_key 113 
        gconftool-2 --set --type int /apps/preenv/driver/key_113_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_113_x 72
        gconftool-2 --set --type int /apps/preenv/driver/key_113_y 313
        gconftool-2 --set --type int /apps/preenv/driver/key_119_key 119 
        gconftool-2 --set --type int /apps/preenv/driver/key_119_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_119_x 132
        gconftool-2 --set --type int /apps/preenv/driver/key_119_y 290
        gconftool-2 --set --type int /apps/preenv/driver/key_101_key 101 
        gconftool-2 --set --type int /apps/preenv/driver/key_101_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_101_x 196
        gconftool-2 --set --type int /apps/preenv/driver/key_101_y 313
        gconftool-2 --set --type int /apps/preenv/driver/key_100_key 100 
        gconftool-2 --set --type int /apps/preenv/driver/key_100_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_100_x 219
        gconftool-2 --set --type int /apps/preenv/driver/key_100_y 370
        gconftool-2 --set --type int /apps/preenv/driver/key_99_key 99 
        gconftool-2 --set --type int /apps/preenv/driver/key_99_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_99_x 196
        gconftool-2 --set --type int /apps/preenv/driver/key_99_y 422
        gconftool-2 --set --type int /apps/preenv/driver/key_120_key 120 
        gconftool-2 --set --type int /apps/preenv/driver/key_120_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_120_x 132
        gconftool-2 --set --type int /apps/preenv/driver/key_120_y 441
        gconftool-2 --set --type int /apps/preenv/driver/key_122_key 122 
        gconftool-2 --set --type int /apps/preenv/driver/key_122_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_122_x 72
        gconftool-2 --set --type int /apps/preenv/driver/key_122_y 422
        gconftool-2 --set --type int /apps/preenv/driver/key_97_key 97 
        gconftool-2 --set --type int /apps/preenv/driver/key_97_mouse 1
        gconftool-2 --set --type int /apps/preenv/driver/key_97_x 46
        gconftool-2 --set --type int /apps/preenv/driver/key_97_y 370
        gconftool-2 --set --type int /apps/preenv/driver/key_105_key 105 
        gconftool-2 --set --type int /apps/preenv/driver/key_105_mouse 2
        gconftool-2 --set --type int /apps/preenv/driver/key_105_x 75
        gconftool-2 --set --type int /apps/preenv/driver/key_105_y 71

        gconftool-2 --set --type int /apps/preenv/driver/key_112_key 112 
        gconftool-2 --set --type int /apps/preenv/driver/key_112_mouse 2
        gconftool-2 --set --type int /apps/preenv/driver/key_112_x 750
        gconftool-2 --set --type int /apps/preenv/driver/key_112_y 111
        gconftool-2 --set --type int /apps/preenv/driver/key_111_key 111 
        gconftool-2 --set --type int /apps/preenv/driver/key_111_mouse 2
        gconftool-2 --set --type int /apps/preenv/driver/key_111_x 742
        gconftool-2 --set --type int /apps/preenv/driver/key_111_y 330
        gconftool-2 --set --type int /apps/preenv/driver/key_108_key 108 
        gconftool-2 --set --type int /apps/preenv/driver/key_108_mouse 2
        gconftool-2 --set --type int /apps/preenv/driver/key_108_x 638
        gconftool-2 --set --type int /apps/preenv/driver/key_108_y 432
elif [ "$1" = "uninstall" ]; then
    #
    # Some uninstall actions
    # 
        gconftool-2 --recursive-unset /apps/preenv/driver
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
