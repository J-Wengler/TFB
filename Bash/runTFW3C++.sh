#!/bin/bash

 g++ -O3 -std=gnu++14 ../TestFixedWidth3/TestFixedWidth3/main.cpp -o TFW3

 time ./TFW3 ../TFB_DATA/100_900_1000000.fwf2.ll ../TFB_DATA/100_900_1000000.fwf2 ../TFB_DATA/100_900_1000000.fwf2.cc ../Output/OutputTFW3++.txt ../TFB_DATA/100_900_1000000.fwf2.mccl ../TFB_DATA/100_900_1000000_columns.tsv 1000000 ../TFB_DATA/100_900_1000000.fwf2.ct 90,100

 echo "C++ Code ^"


