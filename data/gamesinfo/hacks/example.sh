#!/bin/sh

#
# Example Post-Install/Uninstall script for WebOS Game 
# Usage: ./example.sh [install|uninstall] bin_dir data_dir
# On install, invoked as ./example.sh install /home/user/Games/ExampleGame /home/user/MyDocs/Games/Example/Game
# In uninstall: ./example.sh uninstall /home/user/Games/ExampleGame /home/user/MyDocs/Games/Example/Game
#

if [ "$#" -ne "3" ]; then
    echo "Usage: $0 [install/uninstall] bin_dir data_dir"
    exit 1
fi

HACKS_DIR=/usr/share/wgames/gamesinfo/hacks/
BIN_DIR=$2
DATA_DIR=$3

if [ "$1" = "install" ]; then
    #
    # Some install actions
    #
    true
elif [ "$1" = "uninstall" ]; then
    #
    # Some uninstall actions
    # 
    true
else
    echo "First parameter should be 'install' or 'uninstall'."
    exit 2
fi

exit 0
