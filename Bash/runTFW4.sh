#!/bin/bash

g++ -O3 -std=gnu++14 ../TestFixedWidth4/TestFixedWidth4/main.cpp -lz -lzstd -o ./TFW4
./TFW4 /Users/jameswengler/TFB/Transposed/10_90_1000.fwf2.zstd_1 ../TestData/10_90_1000.fwf2 ../TestData/10_90_1000.fwf2_columns.tsv 10,100 ../TestData/Output.txt
