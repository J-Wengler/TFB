#!/bin/bash
echo "Running both versions of code as parallel processes"
(bash ./runTFW3C++.sh) & (bash ./runTFW3PY.sh)
echo 
wait
echo "Testing Files for Differences"
python3 ../Python/CompareFiles.py ../Output/OutputPY.txt ../Output/OutputTFW3++.txt no_line
