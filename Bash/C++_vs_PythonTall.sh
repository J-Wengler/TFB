#!/bin/bash
echo "Running both versions of code as parallel processes"
(bash ./runOld.sh) &
(bash ./runPython.sh)
echo
wait
echo "Testing Files for differences"
python3 ../Python/CompareFiles.py ../Output/OutputPY.txt ../Output/OldOutputC++.txt no_line
