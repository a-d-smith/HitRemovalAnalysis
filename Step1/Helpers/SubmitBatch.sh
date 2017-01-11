#!/bin/bash

# You will need a list of all of your files and the number of events to process in each.
# Each line should contain a file name and the number of events to process separated by a space:
#
#   filename.pndr nEvents 
#
# The lines in the pre and post file-event lists should match 1:1

if [ $# -ne 5 ]; then
    echo "Error: You need to pass the following parameters"
    echo "       1) pre file event list"
    echo "       2) post file event list"
    echo "       3) output directory"
    echo "       4) setup script"
    echo "       5) PandoraInterface binary"
    return 0
fi

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
OUTPUT_DIR=`readlink -f $3`
SETUP=`readlink -f $4`
BINARY=`readlink -f $5`

# Remove the scripts in the Scripts folder
if [ ! -d ${OUTPUT_DIR}/Scripts ]; then
    mkdir ${OUTPUT_DIR}/Scripts
fi
rm -f ${OUTPUT_DIR}/Scripts/*

# Add submission of the pre files
while read line; do
    file=`echo $line | cut -f1 -d' '`
    events=`echo $line | cut -f2 -d' '`

    # Get the file ID of this file
    ID=${file%.pndr}
    ID=${ID##*-}

    # Make the settings xml file
    source ${THIS_DIR}/GetSettings.sh pre $file $ID ${OUTPUT_DIR}/Scripts/SettingsPre-${ID}.xml 

    echo "#!/bin/bash" > ${OUTPUT_DIR}/Scripts/SubmitJobPre-${ID}.sh
    echo "source ${THIS_DIR}/SubmitJob.sh ${OUTPUT_DIR}/Scripts/SettingsPre-${ID}.xml $events ${OUTPUT_DIR} ${SETUP} ${BINARY}" >> ${OUTPUT_DIR}/Scripts/SubmitJobPre-${ID}.sh
done < $1

# Add submission of the post files
while read line; do
    file=`echo $line | cut -f1 -d' '`
    events=`echo $line | cut -f2 -d' '`

    # Get the file ID of this file
    ID=${file%.pndr}
    ID=${ID##*-}

    # Make the settings xml file
    source ${THIS_DIR}/GetSettings.sh post $file $ID ${OUTPUT_DIR}/Scripts/SettingsPost-${ID}.xml 

    echo "#!/bin/bash" > ${OUTPUT_DIR}/Scripts/SubmitJobPost-${ID}.sh
    echo "source ${THIS_DIR}/SubmitJob.sh ${OUTPUT_DIR}/Scripts/SettingsPost-${ID}.xml $events ${OUTPUT_DIR} ${SETUP} ${BINARY}" >> ${OUTPUT_DIR}/Scripts/SubmitJobPost-${ID}.sh
done < $2

echo "executable     = \$(filename)" > ${OUTPUT_DIR}/Scripts/JobSubmitter
echo "log            = \$(filename).log" >> ${OUTPUT_DIR}/Scripts/JobSubmitter
echo "requirements   = (OSTYPE == \"SLC6\") && (LoadAvg < 0.5)" >> ${OUTPUT_DIR}/Scripts/JobSubmitter
echo "request_memory = 1024" >> ${OUTPUT_DIR}/Scripts/JobSubmitter
echo "rank           = memory" >> ${OUTPUT_DIR}/Scripts/JobSubmitter
echo "universe       = vanilla" >> ${OUTPUT_DIR}/Scripts/JobSubmitter
echo "input          = /dev/null" >> ${OUTPUT_DIR}/Scripts/JobSubmitter
echo "queue filename matching files *.sh" >> ${OUTPUT_DIR}/Scripts/JobSubmitter

PWD_DIR=`pwd`
cd ${OUTPUT_DIR}/Scripts
condor_submit JobSubmitter
cd $PWD_DIR

