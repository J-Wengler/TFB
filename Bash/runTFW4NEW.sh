#!/bin/bash

g++ -O3 -std=gnu++14 ../TestFixedWidth4/TestFixedWidth4/main.cpp -lz -lzstd -o ./TFW4

./TFW4 /Users/jameswengler/TFB/DataPath/100_900_1000000.fwf2.zstd_1 /Users/jameswengler/TFB/TransposedDataPath/100_900_1000000.fwf2.zstd_1 ../TestData/100_900_1000000.fwf2_columns.tsv ../TestData/OutputNEW.txt 100,1000
