#!/bin/bash
echo
echo "Testing Old Code"
echo "Compilation time (-std=gnu++14):"

time clang++ -std=gnu++14 ../mmap_test/mmap_test/main.cpp -o mmap

echo
echo "C++ File:
"
time ./mmap /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.ll /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.cc /Users/jameswengler/TFB/Output/OldOutputC++.txt /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.mccl /Users/jameswengler/TFB/TFB_DATA/100_900_1000000_columns.tsv 1000000
echo
echo "Testing New Code"
echo "Compilation time (-std=gnu++14):"

time clang++ -std=gnu++14 ../mmap_test_dev/mmap_test_dev/main.cpp -o NewMmap

echo
echo "C++ File:
"
time ./NewMmap /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.ll /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.cc /Users/jameswengler/TFB/Output/NewOutputC++.txt /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.mccl /Users/jameswengler/TFB/TFB_DATA/100_900_1000000_columns.tsv 1000000

echo
echo "Testing Files for differences"
python ../Python/CompareFiles.py ../Output/OldOutputC++.txt ../Output/NewOutputC++.txt no_line




