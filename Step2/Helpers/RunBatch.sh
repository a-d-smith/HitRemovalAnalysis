#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Error: Please pass the following as parameters"
    echo "       1) Pre file regex (use quotes)"
    echo "       2) Post file regex (use quotes)"
    echo "       3) Results directory"
    return 0
fi

# First get a list of all the files provided by the regex
rm -f /tmp/prefiles
for f in $1; do
    readlink -f $f >> /tmp/prefiles
done

rm -f /tmp/postfiles
for f in $2; do
    readlink -f $f >> /tmp/postfiles
done

# Sanity check that the number of files is the same for pre and post
if [ `wc -l /tmp/prefiles | cut -f1 -d' '` -ne `wc -l /tmp/postfiles | cut -f1 -d' '` ]; then
    echo "Error: Number of pre and post files don't match!"
    return 0
fi

NFILES=`wc -l /tmp/prefiles | cut -f1 -d' '`

RESULTS_DIR=`readlink -f $3`

# Get the directory in which this script exists
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# If everything has gone well, the lines in each pre / post file list should match up!
for i in `seq 1 $NFILES`; do
    prefile=`sed $i'q;d' /tmp/prefiles`
    postfile=`sed $i'q;d' /tmp/postfiles`

    # Get the ID of each of the files
    ID_PRE=${prefile%.root}
    ID_PRE=${ID_PRE##*_}
    ID_POST=${postfile%.root}
    ID_POST=${ID_POST##*_}

    # Check the IDs match
    if [ $ID_PRE -ne $ID_POST ]; then
        echo "Error: File IDs do not match!"
        return 0
    fi

    # Run the code
    source $SCRIPT_DIR/RunJob.sh $prefile $postfile $RESULTS_DIR/MergedData_${ID_PRE}.root
done
