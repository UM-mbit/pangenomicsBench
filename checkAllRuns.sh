#!/usr/bin/bash

#This script just cats the results files that were created for each kernel

# Check if directory is provided as an argument
if [ $# -eq 0 ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

inputDir=$1

# Iterate over all subdirectories in the provided directory
for d in "$inputDir"/*; do
    echo ""
    echo "Checking $d"
    ls $d/Results
    #grep -riI "warning" $d/Logs
done
