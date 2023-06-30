#!/bin/bash
#set -xe # debug logs and exit on error

#start the target service to run make and clean up the container on exit
#change the working directory within the container to target Makefile location


# This script runs build commands through docker-compose.
#      -f file to use, but default is docker-compose.yml anyway, or at least one of the defaults.
#      run     a one-off command;  IN A DOCKER CONTAINER.
#      --rm    remove the container once we are done.
#      -w    same alias/directory as in the Docker-compose file.
#      service name, listed in the yml file.
#      THEN ALL THE COMMAND LINE ARGS TO PERFORM THE BUILD
docker-compose -f ./docker/docker-compose.yml run --rm \
    -w /dirDockerProjAlias/build \
    L6DisplayDockerService \
    make \
    -r -j all
