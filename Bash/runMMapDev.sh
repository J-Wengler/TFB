#!/bin/bash

clang++ -std=gnu++14 ../mmap_test_dev/mmap_test_dev/main.cpp -o NewMmap
 ./NewMmap /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.ll /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2 /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.cc /Users/jameswengler/TFB/Output/NewOutputC++.txt /Users/jameswengler/TFB/TFB_DATA/100_900_1000000.fwf2.mccl /Users/jameswengler/TFB/TFB_DATA/100_900_1000000_columns.tsv 1000000
