#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Error: Please pass the following as parameters"
    echo "       1) Merged file regex (use quotes)"
    echo "       2) Results directory"
    echo "       3) ROOT setup script"
    return 0
fi

# First get a list of all the files provided by the regex
rm -f /tmp/mergedfiles
for f in $1; do
    readlink -f $f >> /tmp/mergedfiles
done

NFILES=`wc -l /tmp/prefiles | cut -f1 -d' '`

RESULTS_DIR=`readlink -f $2`
ROOT_SETUP=`readlink -f $3`

# Get the directory in which this script exists
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


# Remove the scripts in the Scripts folder
if [ ! -d ${RESULTS_DIR}/Scripts ]; then
    mkdir ${RESULTS_DIR}/Scripts
fi
rm -f ${RESULTS_DIR}/Scripts/*


# If everything has gone well, the lines in each pre / post file list should match up!
for i in `seq 1 $NFILES`; do
    mergedfile=`sed $i'q;d' /tmp/mergedfiles`

    # Get the ID of each of the files
    ID=${mergedfile%.root}
    ID=${ID##*_}

    # Run the code
    echo "#!/bin/bash" > ${RESULTS_DIR}/Scripts/SubmitNTuples-${ID}.sh
    echo "cd $RESULTS_DIR" >> ${RESULTS_DIR}/Scripts/SubmitNTuples-${ID}.sh
    echo "source $SCRIPT_DIR/RunJobAssociations.sh $mergedfile $ID $ROOT_SETUP" >> ${RESULTS_DIR}/Scripts/SubmitNTuples-${ID}.sh
done

# Make all of the single job running scripts executable
chmod u+x ${RESULTS_DIR}/Scripts/SubmitNTuples*.sh

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
