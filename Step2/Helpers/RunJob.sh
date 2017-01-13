#!/bin/bash

if [ $# -ne 4 ]; then
    echo "Error: Please pass the following as parameters"
    echo "       1) Pre file"
    echo "       2) Post file"
    echo "       3) Output file (include .root)"
    echo "       4) Root setup script"
    echo "       5) Number of good events (at least one neutrino hit) - Optional"
    return 0
fi

# Get the directory in which this script exists
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PRE_FILE=`readlink -f $1`
POST_FILE=`readlink -f $2`
OUTPUT_FILE=`readlink -f $3`
ROOT_SETUP=`readlink -f $4`

source $ROOT_SETUP

# Nasty code to run root
if [ $# == 5 ]; then
    NEVENTS=$5
    echo 'gROOT->LoadMacro("'$SCRIPT_DIR'/../Merging/Merging.C"); Merge("'$PRE_FILE'","'$POST_FILE'","'$OUTPUT_FILE'",'$NEVENTS'); gSystem->Exit(0);' | root -b -l
else
    echo 'gROOT->LoadMacro("'$SCRIPT_DIR'/../Merging/Merging.C"); Merge("'$PRE_FILE'","'$POST_FILE'","'$OUTPUT_FILE'"); gSystem->Exit(0);' | root -b -l
fi
