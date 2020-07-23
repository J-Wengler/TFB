#!/bin/bash

g++ -g -std=gnu++14 ../TestFixedWidth4/TestFixedWidth4/main.cpp -lz -lzstd
lldb -- ./a.out ~/TempDir/Temp/10_90_1000.fwf2.zstd_1 ~/TempDir/Temp/10_90_1000.fwf2 ~/TempDir/Temp/10_90_1000_columns.tsv ~/TempDir/Temp/Output.txt 10,100
