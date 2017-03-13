#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Error: Please pass the following as parameters"
    echo "       1) Merged .root file"
    echo "       2) Output file id"
    echo "       3) Root setup script"
    return 0
fi

# Get the directory in which this script exists
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

MERGED_FILE=`readlink -f $1`
ROOT_SETUP=`readlink -f $3`

source $ROOT_SETUP

# Nasty code to run root
echo 'gROOT->LoadMacro("'$SCRIPT_DIR'/../NTuples/LoadMergedData.C"); gROOT->ProcessLine(".x '$SCRIPT_DIR'/../NTuples/getMCParticleAssociations.C(\"'$MERGED_FILE'\",'${2}')"); gSystem->Exit(0);' | root -b -l
