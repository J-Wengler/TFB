#!/bin/bash
echo "Running both versions of code as parallel processes"
echo  -e "\033[33;5mWaiting\033[0m"
(bash ./runNew.sh) &
(bash ./runPython.sh)
echo
wait
echo "Testing Files for differences"
python3 ../Python/CompareFiles.py ../Output/OutputPY.txt ../Output/NewOutputC++.txt no_line
