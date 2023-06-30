#!/bin/bash
set -xe # debug logs and exit on error

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=$(dirname $SCRIPT)

# assign variables
BRANCH="$1"
BASE="$2"
KEY="$3"
NUM_PROC=$(nproc)

# navigate to root repo directory
cd $SCRIPT_DIR && cd ../../
REPO_DIR=$(pwd)

#  for use with the sonar cache, which it insists upon using.
#  need to start at the top-most directory
SONAR_USER_HOME=/tmp
export  SONAR_USER_HOME


# This shell script runs teamcity build steps inside the docker container
# for sonarqube and sonarqube code coverage

# sonarqube wrapper required around the debug build
rm -rf ./build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=./proj/cmake/arm-cortex.cmake
cd ./build
build-wrapper-linux-x86-64 --out-dir /tmp/ make -r -j all

cd $REPO_DIR

if [ "$BASE" == "%teamcity.pullRequest.target.branch%" ]
#Run Sonarqube for develop
then
sonar-scanner \
    -Dproject.settings=./proj/docker/sonarqube.properties \
    -Dsonar.cfamily.threads=$NUM_PROC \
    -Dsonar.branch.name="$BRANCH" \
    -Dsonar.exclusions="
        **/docker/**,
        **/dredd/**/*,
        **/TDD/**/*,
        **/test/**/*,
        **/build/**/*,
        **/*.py
#   was on former release, but never ran SQ        **/ide_library/**
    "
else
#Run Sonarqube for PR
sonar-scanner \
    -Dproject.settings=./proj/docker/sonarqube.properties \
    -Dsonar.cfamily.threads=$NUM_PROC \
    -Dsonar.pullrequest.branch="$BRANCH" \
    -Dsonar.pullrequest.base="$BASE" \
    -Dsonar.pullrequest.key="$KEY" \
    -Dsonar.scm.disabled="false" \
    -Dsonar.exclusions="
        **/docker/**,
        **/dredd/**/*,
        **/TDD/**/*,
        **/test/**/*,
        **/build/**/*,
        **/*.py
#   was on former release, but never ran SQ        **/ide_library/**
    "
fi
