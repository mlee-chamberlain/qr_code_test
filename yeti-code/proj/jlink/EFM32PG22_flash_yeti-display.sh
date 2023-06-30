#!/bin/bash
DEVICE="EFM32PG22CXXXF64"
INTERFACE="SWD"
SPEED="4000"

JLINKSCRIPT="./proj/jlink/EFM32PG22CXXXF64.jlink"

# Validate Path and files
if [ ! -z $(pwd | grep "jlink") ]; then
    cd ../..
fi

if [ ! -s $YETI_DISPLAY_BIN ]; then
	echo "Couldn't find a binary: ${YETI_DISPLAY_BIN}"
	exit 1
fi

if [ ! -s $JLINKSCRIPT ]; then
	echo "Couldn't find Jlink script ${JLINKSCRIPT}"
	exit 1
fi

# Run the actual flash process
JLinkExe -Device $DEVICE -If $INTERFACE -Speed $SPEED -autoconnect 1 -CommandFile $JLINKSCRIPT
