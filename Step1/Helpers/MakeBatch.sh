#!/bin/bash

# For this script to work, your files must end in a hypen followed by a number followed by .pndr  eg. myfile-10.pndr
# You will also need a list of all of your files and the number of events to process in each 

if [ $# -ne 2 ]; then
    echo "Error: You need to pass the following parameters"
    echo "       1) pre files regex"
    echo "       2) post files regex"
fi


find /r05/uboone/asmith/bnb_cosmic_samples/high_stats/1001/cosmic1/* -type f | sort

