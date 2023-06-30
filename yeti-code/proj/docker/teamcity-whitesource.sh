#!/bin/bash
set -xe # debug logs and exit on error

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=$(dirname $SCRIPT)

# navigate to root level repo directory
cd $SCRIPT_DIR && cd ../../

# Grab the JAR file for whitesource from Oakfs
cp /oakfs01/Eng/shared/SHARED/FirmwareEngineering/Whitesource/wss-unified-agent.jar ./

# Pass to whitesource (in docker container)
$SCRIPT_DIR/build.sh java -jar ./wss-unified-agent.jar -c ./proj/docker/wss-unified-agent.config -d ./