#!/bin/sh

#
# Alternate wrapper script which allows using
# another (jflatt's one) version of preenv.
# The main reason for it is multitouch keyboard
# remapping support. As soon as it will be merged
# into main preenv branch, this script will be
# removed.
#

export WGAMES_LIBS="/usr/share/wgames/lib/"
export PREENV_ROOT="/opt/preenv/"
export PREENV_APPID="$1"

if [ -n "$PATH" ]; then
	export PATH="$PREENV_ROOT/bin:$PATH"
else
	export PATH="$PREENV_ROOT/bin"
fi

if [ -n "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH="$WGAMES_LIBS:$PREENV_ROOT/lib:$LD_LIBRARY_PATH"
else
	export LD_LIBRARY_PATH="$WGAMES_LIBS:$PREENV_ROOT/lib"
fi

if [ -n "$LD_PRELOAD" ]; then
	export LD_PRELOAD="$WGAMES_LIBS/libsdlpre.so:$LD_PRELOAD"
else
	export LD_PRELOAD="$WGAMES_LIBS/libsdlpre.so"
fi

cd "$(dirname "$2")"
exec "$2"
