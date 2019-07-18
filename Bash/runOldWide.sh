#!/bin/bash
g++ -std=gnu++14 ../mmap_test/mmap_test/main.cpp -o mmap 
time ./mmap ../TFB_DATA/100000_900000_1000.fwf2.ll ../TFB_DATA/100000_900000_1000.fwf2 ../TFB_DATA/100000_900000_1000.fwf2.cc ../Output/OldOutputC++.txt ../TFB_DATA/100000_900000_1000.fwf2.mccl ../TFB_DATA/100000_900000_1000_columns.tsv 1000
echo "Old Code ^"
