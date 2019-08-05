#!/bin/bash
echo "Running both versions of code as parallel processes"
(bash ./runOldWide.sh) &
(bash ./runPythonWide.sh)
echo
wait
echo "Testing Files for differences"
python3 ../Python/CompareFiles.py ../Output/OutputPY.txt ../Output/OldOutputC++.txt no_line
