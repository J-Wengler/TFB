#!/bin/bash

g++ -g -std=gnu++14 ../TestFixedWidth4/TestFixedWidth4/main.cpp -lzstd

lldb -- ./a.out /Users/jameswengler/TFB/Transposed/10_90_1000.fwf2 ../TestData/10_90_1000.fwf2 ../TestData/10_90_1000.fwf2_columns.tsv 10,100 ../TestData/Output.txt
