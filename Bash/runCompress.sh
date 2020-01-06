#!/bin/bash

g++ -O3 ../TestFixedWidth4/TestFixedWidth4/main.cpp -lzstd 

./a.out ../Transposed/10_90_1000.fwf2.zstd_1


