#!/bin/bash
g++ -O3 -std=gnu++14 ../mmap_test_dev/mmap_test_dev/main.cpp -o NewMmap 
time ./NewMmap ../TFB_DATA/100000_900000_1000.fwf2.ll ../TFB_DATA/100000_900000_1000.fwf2 ../TFB_DATA/100000_900000_1000.fwf2.cc ../Output/NewOutputC++.txt ../TFB_DATA/100000_900000_1000.fwf2.mccl ../TFB_DATA/100000_900000_1000_columns.tsv 1000
echo "New Code^"
