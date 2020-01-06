#!/bin/bash

g++ -g ../TestFixedWidth4/TestFixedWidth4/main.cpp -lzstd 

lldb -- ./a.out ../Transposed/10_90_1000.fwf2.zstd_1
