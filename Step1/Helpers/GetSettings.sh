#!/bin/bash

if [ $# -ne 4 ]; then
    echo "Please pass the following arguments"
    echo "  1) pre / post"
    echo "  2) input file"
    echo "  3) file id "
    echo "  4) output file"
    return 0
fi

# Work out if we are making a pre or post file
PRE_POST=-1
if [ $1 == "pre" ]; then
    PRE_POST=0
fi
if [ $1 == "post" ]; then
    PRE_POST=1
fi
if [ $PRE_POST == -1 ]; then
    echo "Error: You have to give either pre or post as the first argument"
    return 0
fi

# Get the directory in which this script exists
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Copy the generic settings file
if [ $PRE_POST == 0 ]; then
    cp $SCRIPT_DIR/../Settings/PandoraSettings_MicroBooNE_Pre_Validation.xml $4 
else
    cp $SCRIPT_DIR/../Settings/PandoraSettings_MicroBooNE_Post_Validation.xml $4
fi

# Modify the input file name
sed -i 's/INPUT_FILE_NAME/'$2'/g' $4
sed -i 's/FILE_ID/'$3'/g' $4




