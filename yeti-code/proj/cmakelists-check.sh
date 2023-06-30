#!/bin/bash
# @author Ivan Jauregui
# @email ijauregu@chamberlain.com
#        ijauregui@luxoft.com
#
# @project yeti-display
#
# This script search if all the C files in project, exist in
# CMakelists files in a basic check.
#
# RUN IT FROM PROJECT'S ROOT FOLDER PREFERABLY.
#
# Limitations:
#  - For now it doesn't check for h include paths
#  - If a .c file is moved to another location, it won't complain
#  - OR If a CmakeLists.txt file is moved to another folder or path
#    the script won't complain of missing .c files in txts.
#  - No particular file(s) exclusions handled for now, maybe in the future.
#  - There might be other limitations.

FOLDER=$(pwd | grep "proj")
if [[ ! -z $FOLDER ]]; then
    # Running from proj folder
    cd ..
fi

echo "Running from $(pwd)"

# Search all .c & .s files in project, excluding build folder
issues=0
for f in $(find . \( -iname \*.c -o -iname \*.s \) ! -path "./build/*" ); do

    file=$(echo $f | rev | cut -d"/" -f1 | rev)

        for i in $(echo $file); do
            if [[ -z $(egrep -r $i --include=CMakeLists.txt) ]]; then
                echo "I couldn't find ${i} in CMakeLists:"
                find . -name "${i}"
                echo
                issues=$(( $issues + 1 )) # increase issues number
            fi
        done

done

echo "${issues} issue(s) found."
