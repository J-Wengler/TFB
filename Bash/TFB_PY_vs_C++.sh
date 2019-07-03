#!/bin/bash

#echo
#echo "Python File:
#"
#time python TestFixedWidth2.py /Users/jameswengler/TFB/TFB_DATA/100000_900000_1000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100000_900000_1000_columns.tsv ~/TFB/OutputPY.txt 1000
#
#echo
#echo "Compilation time (-std=gnu++14):"
#
#time clang++ -std=gnu++14 mmap_test/mmap_test/main.cpp -o mmap
##time g++ -std=gnu++14 mmap_test/mmap_test/main.cpp -o mmap
#
#echo
#echo "C++ File:"
#
#time ./mmap /Users/jameswengler/TFB/TFB_DATA/100000_900000_1000.fwf2.ll /Users/jameswengler/TFB/TFB_DATA/100000_900000_1000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100000_900000_1000.fwf2.cc /Users/jameswengler/TFB/OutputC++.txt /Users/jameswengler/TFB/TFB_DATA/100000_900000_1000.fwf2.mccl /Users/jameswengler/TFB/TFB_DATA/100000_900000_1000_columns.tsv 1000

#python programStats.py Python Py_Output.txt
#python programStats.py C++ C++_Output.txt

echo
echo "Python File:
"
time python ./Python/TestFixedWidth2.py /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100_900_1000000_columns.tsv ~/TFB/OutputPY.txt 1000000

echo
echo "Compilation time (-std=gnu++14):"

time clang++ -std=gnu++14 mmap_test/mmap_test/main.cpp -o mmap

echo
echo "C++ File:
"
time ./mmap /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.ll /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.cc /Users/jameswengler/TFB/Output/OutputC++.txt /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.mccl /Users/jameswengler/TFB/TFB_DATA/100_900_1000000_columns.tsv 1000000
