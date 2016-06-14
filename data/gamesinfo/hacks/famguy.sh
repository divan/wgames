#!/bin/sh

#
# Driver Post-Install/Uninstall script for WebOS Game Family Guy Uncensored 
# Usage: ./famguy.sh [install|uninstall] bin_dir data_dir
#

if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Create powervr config
    echo -e "[default]\nForceExternalZBuffer=0\n" > ${BIN_DIR}/famguy.ini
    #
    # Configure keyboard mapping
    #
    gconftool-2 --recursive-unset /apps/preenv/famguy.exe
    gconftool-2 --set --type int /apps/preenv/famguy.exe/enable_dpad 0
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_115_key 115 # s = Left / Jump / Reverse
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_115_mouse 2
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_115_x 85
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_115_y 426
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_100_key 100 # d = Right 
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_100_mouse 2
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_100_x 215
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_100_y 420    
    
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_107_key 107 # k = Button A
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_107_mouse 1
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_107_x 610
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_107_y 420
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_108_key 108 # a = Button B
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_108_mouse 1
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_108_x 750
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_108_y 420

    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_109_key 109 # m = Menu
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_109_mouse 2
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_109_x 17
    gconftool-2 --set --type int /apps/preenv/famguy.exe/key_109_y 14
             
elif [ "$1" = "uninstall" ]; then
    #
    # Some uninstall actions
    # 
    gconftool-2 --recursive-unset /apps/preenv/famguy.exe
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0