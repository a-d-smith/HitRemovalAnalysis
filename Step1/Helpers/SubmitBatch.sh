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

# Remove the scripts in the Scripts folder
if [ ! -d $3/Scripts ]; then
    mkdir $3/Scripts
fi
rm -f $3/Scripts/*

# Add submission of the pre files
while read line; do
    file=`echo $line | cut -f1 -d' '`
    events=`echo $line | cut -f2 -d' '`

    # Get the file ID of this file
    ID=${file%.pndr}
    ID=${ID##*-}

    # Make the settings xml file
    source ${THIS_DIR}/GetSettings.sh pre $file $ID $3/Scripts/SettingsPre-${ID}.xml 

    echo "source ${THIS_DIR}/SubmitJob.sh $3/Scripts/SettingsPre-${ID}.xml $events $3 $4 $5" > $3/Scripts/SubmitJobPre-${ID}.sh
done < $1

# Add submission of the post files
while read line; do
    file=`echo $line | cut -f1 -d' '`
    events=`echo $line | cut -f2 -d' '`

    # Get the file ID of this file
    ID=${file%.pndr}
    ID=${ID##*-}

    # Make the settings xml file
    source ${THIS_DIR}/GetSettings.sh post $file $ID $3/Scripts/SettingsPost-${ID}.xml 

    echo "source ${THIS_DIR}/SubmitJob.sh $3/Scripts/SettingsPost-${ID}.xml $events $3 $4 $5" > $3/Scripts/SubmitJobPost-${ID}.sh
done < $2

echo "Executable   = \$(filename)" > $3/Scripts/JobSubmitter
echo "queue filename matching files *.sh" >> $3/Scripts/JobSubmitter

