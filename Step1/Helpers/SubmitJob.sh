#!/bin/bash

# Check we have the right number of arguments
if [ $# -ne 5 ]; then
    echo "Error: You need to pass the following arguments:"
    echo "       1) Settings xml files"
    echo "       2) Number of events"
    echo "       3) Output directory"
    echo "       4) Setup script"
    echo "       5) PandoraInterface binary"
    return 0
fi

# Source the setup script
source $4

# Run Pandora
$5 -i $1 -d uboone -n $2 -N

# Move output to the correct directory
mv HitData_* $3/.

