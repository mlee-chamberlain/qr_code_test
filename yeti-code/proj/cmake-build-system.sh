#!/bin/bash
#set -xe # debug logs and exit on error


# This script runs build commands through docker-compose.
#      -f file to use, but default is docker-compose.yml anyway, or at least one of the defaults.
#      run     a one-off command;  IN A DOCKER CONTAINER.
#      --rm    remove the container once we are done.
#      service name, listed in the yml file.
#      THEN ALL THE COMMAND LINE ARGS TO PERFORM THE CMAKE.
docker-compose -f ./docker/docker-compose.yml run --rm \
    L6DisplayDockerService \
    cmake \
    -S . \
    -B build \
    -DCMAKE_TOOLCHAIN_FILE=./proj/cmake/arm-cortex.cmake 
