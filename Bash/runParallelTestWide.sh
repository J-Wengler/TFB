#!/bin/bash
echo "Running both versions of code as parallel processes"
(bash ./runOldWide.sh) &
(bash ./runNewWide.sh)
echo
wait
echo "Testing Files for differences"
 python3 ../Python/CompareFiles.py ../Output/OldOutputC++.txt ../Output/NewOutputC++.txt no_line
