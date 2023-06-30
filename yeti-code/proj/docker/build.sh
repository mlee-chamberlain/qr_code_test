#!/bin/bash
#set -xe # debug logs and exit on error

#  gets the directory of this file.
SCRIPT=$(readlink -f $0)
SCRIPT_DIR=$(dirname $SCRIPT)

# navigate to /proj repo directory
cd $SCRIPT_DIR

# setup some ID's for use later in a container.
#  Same user as already running this script.
export  UID
export  GID=$(id -g)

# This script runs build commands through docker-compose.
#      -f file to use, but default is docker-compose.yml anyway, or at least one of the defaults.
#      run     a one-off command;  IN A DOCKER CONTAINER.
#      --rm    remove the container once we are done.
#      service name, listed in the yml file.
#      BASH, THEN ALL THE COMMAND LINE ARGS TO THIS FILE WHEN IT WAS INVOKED.
docker-compose -f ./docker-compose.yml run --rm   L6DisplayDockerService  bash -c "$*"
