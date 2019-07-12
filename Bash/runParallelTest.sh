#!/bin/bash
echo "Running both versions of code as parallel processes"
echo  -e "\033[33;5mWaiting\033[0m"
(bash ./runOld.sh) &
(bash ./runNew.sh)
echo
echo "Testing Files for differences"
python ../Python/CompareFiles.py ../Output/OldOutputC++.txt ../Output/NewOutputC++.txt no_line
