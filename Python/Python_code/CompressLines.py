import mmap
import os
import shutil
import sys
from Helper import *

file_path = sys.argv[1]
num_rows = int(sys.argv[2])
compression_method = sys.argv[3]
compression_level = sys.argv[4]
is_transposed = sys.argv[5] == "True"

if compression_method == "bz2":
    import bz2 as cmpr
    compression_code = "cmpr.compress(line, compresslevel={})".format(compression_level)
    out_file_path = file_path + "." + compression_method + "_" + compression_level
elif compression_method == "gz":
    import gzip as cmpr
    compression_code = "cmpr.compress(line, compresslevel={})".format(compression_level)
    out_file_path = file_path + "." + compression_method + "_" + compression_level
elif compression_method == "lzma":
    import lzma as cmpr
    compression_code = "cmpr.compress(line)"
    out_file_path = file_path + "." + compression_method
elif compression_method == "snappy":
    import snappy as cmpr
    compression_code = "cmpr.compress(line)"
    out_file_path = file_path + "." + compression_method
elif compression_method == "zstd":
    # See https://pypi.org/project/zstandard
    import zstandard
    cmpr = zstandard.ZstdCompressor(level=int(compression_level))
    compression_code = "cmpr.compress(line)"
    out_file_path = file_path + "." + compression_method + "_" + compression_level
elif compression_method == "lz4":
    # See https://python-lz4.readthedocs.io/en/stable/quickstart.html#simple-usage
    import lz4.frame as cmpr
    compression_code = "cmpr.compress(line)"
    out_file_path = file_path + "." + compression_method + "_" + compression_level
else:
    print("No matching compression method")
    sys.exit(1)

with open(file_path + ".ll", 'rb') as ll_file:
    line_length = int(ll_file.read().rstrip())

in_row_indices = list(range(num_rows + 1))

out_row_starts = []
cumulative_position = 0

with open(file_path, 'rb') as my_file:
    mmap_file = mmap.mmap(my_file.fileno(), 0, prot=mmap.PROT_READ)

    with open(out_file_path, 'wb') as out_file:
        for row_index in in_row_indices:
            row_start = row_index * line_length

            if row_index == in_row_indices[-1]:
                line = mmap_file[row_start:len(mmap_file)]
            else:
                row_end = (row_index + 1) * line_length
                line = mmap_file[row_start:row_end]

            compressed_line = eval(compression_code)

            out_file.write(compressed_line)
            out_row_starts.append(cumulative_position)
            cumulative_position += len(compressed_line)

# Serialize and save values that indicate where each row starts
out_string, max_length = buildStringMap(out_row_starts)

with open(out_file_path + ".rowstart", 'wb') as rowstart_file:
    rowstart_file.write(out_string)

with open(out_file_path + ".mrsl", 'wb') as mrsl_file:
    mrsl_file.write(max_length)

if is_transposed:
    for file_extension in [".ll", ".cc", ".mccl"]:
        shutil.copyfile(file_path + file_extension, out_file_path + file_extension)
else:
    for file_extension in [".ll", ".cc", ".mccl", ".ct", ".mctl"]:
        shutil.copyfile(file_path + file_extension, out_file_path + file_extension)
