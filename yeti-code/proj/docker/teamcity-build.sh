#!/bin/bash
# debug logs [prints cmds and args], and exits on error [non-zero status]
#set -xe

#  gets the command line for this file
SCRIPT=$(readlink -f $0)

#  gets the directory of this file.
SCRIPT_DIR=$(dirname $SCRIPT)

# navigate to /proj repo directory 
# GETS US TO THIS DIRECTORY AND UP ONE FROM ANYWHERE.
cd $SCRIPT_DIR && cd ../

# setup some ID's for use later in a container.
#  Same user as already running this script.
export  UID
export  GID=$(id -g)

# clean first
./docker/build.sh rm -rf ./build

# This shell script runs teamcity build steps inside the docker container
# run the standard compile and testing for develop or branches
# FIRST DO THE CMAKE...
./cmake-build-system.sh 

# READY TO BUILD
./build-target.sh

