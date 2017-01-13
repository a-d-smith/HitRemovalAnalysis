#!/bin/bash

if [ $# -ne 4 ]; then
    echo "Error: Please pass the following as parameters"
    echo "       1) Pre file regex (use quotes)"
    echo "       2) Post file regex (use quotes)"
    echo "       3) Results directory"
    echo "       4) ROOT setup script"
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
ROOT_SETUP=`readlink -f $4`

# Get the directory in which this script exists
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


# Remove the scripts in the Scripts folder
if [ ! -d ${RESULTS_DIR}/Scripts ]; then
    mkdir ${RESULTS_DIR}/Scripts
fi
rm -f ${RESULTS_DIR}/Scripts/*


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
    echo "#!/bin/bash" > ${RESULTS_DIR}/Scripts/SubmitMerge-${ID_PRE}.sh
    echo "source $SCRIPT_DIR/RunJob.sh $prefile $postfile $RESULTS_DIR/MergedData_${ID_PRE}.root $ROOT_SETUP" >> ${RESULTS_DIR}/Scripts/SubmitMerge-${ID_PRE}.sh
done

# Make all of the single job running scripts executable
chmod u+x ${RESULTS_DIR}/Scripts/SubmitMerge*.sh

echo "executable     = \$(filename)" > ${RESULTS_DIR}/Scripts/JobSubmitter
echo "log            = \$(filename).log" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "requirements   = (OSTYPE == \"SLC6\") && (LoadAvg < 0.5)" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "request_memory = 1024" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "rank           = memory" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "universe       = vanilla" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "input          = /dev/null" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "output         = /dev/null" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "run_as_owner   = true" >> ${RESULTS_DIR}/Scripts/JobSubmitter
echo "queue filename matching files *.sh" >> ${RESULTS_DIR}/Scripts/JobSubmitter

PWD_DIR=`pwd`
cd ${RESULTS_DIR}/Scripts
condor_submit JobSubmitter
cd $PWD_DIR
