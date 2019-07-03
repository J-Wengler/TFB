#!/bin/bash

clang++ -std=gnu++14 ../mmap_test/mmap_test/main.cpp -o mmap
./mmap /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.ll /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.cc /Users/jameswengler/TFB/Output/OldOutputC++.txt /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.mccl /Users/jameswengler/TFB/TFB_DATA/100_900_1000000_columns.tsv 1000000
