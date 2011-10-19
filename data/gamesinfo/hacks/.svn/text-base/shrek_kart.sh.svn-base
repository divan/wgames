#!/bin/sh

#
# Post-Install/Uninstall script for WebOS Game Shrek Kart 
# Usage: ./shrek_kart.sh [install|uninstall] bin_dir data_dir
#


if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    # Create powervr config
    echo -e "[default]\nForceExternalZBuffer=0\n" > ${BIN_DIR}/ShrekKarting.ini
elif [ "$1" = "uninstall" ]; then
    true
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
