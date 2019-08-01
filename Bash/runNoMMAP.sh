#!/bin/bash
g++  -O3 -std=gnu++14 ../TestFixedWidth2_NOMMAP/TestFixedWidth2_NOMMAP/main.cpp -o NO_Mmap

time ./NO_Mmap ../TFB_DATA/100_900_1000000.fwf2.ll ../TFB_DATA/100_900_1000000.fwf2 ../TFB_DATA/100_900_1000000.fwf2.cc ../Output/NoMMapOutputC++.txt ../TFB_DATA/100_900_1000000.fwf2.mccl ../TFB_DATA/100_900_1000000_columns.tsv 1000000 
echo "Using Lseek^"
