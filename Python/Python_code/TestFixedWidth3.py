import mmap
import sys
from Helper import *
import fastnumbers

file_path = sys.argv[1]
col_names_file_path = sys.argv[2]
out_file_path = sys.argv[3]
num_rows = int(sys.argv[4])
query_col_indices = [int(x) for x in sys.argv[5].split(",")]
memory_map = True

def filter_rows(row_indices, query_col_index, query_col_coords): # Pretty sure this returns an array with the rows that have been queried out of the column
    col_type = next(parse_data_values(query_col_index, max_column_type_length, [(query_col_index, 0, 1)], file_handles["ct"])) # Returns the next item from a iterator, which is the "string_like_object" from parse_data_values?

    if col_type == b"n": # why are we treating it as bytes? Speed? I think this is evaluating if the next column is a number
        for row_index in row_indices:
            if fastnumbers.float(next(parse_data_values(row_index, line_length, query_col_coords, file_handles["data"]))) >= 0.1:  # This line confuses me, is it projecting all columns with a numberiv value greater than .1?
                yield row_index
    else:
        for row_index in row_indices:
            value = next(parse_data_values(row_index, line_length, query_col_coords, file_handles["data"])) # What is the purpose of the file handles?

            if value.startswith(b"A") or value.endswith(b"Z"): # Filters all data that starts with A or ends with Z?
                yield row_index

file_handles = { # I don't understand the mechanism of this dictionary,is it taking the place of the format strings in TFW2?
    "cc": openReadFile(file_path, ".cc"),
    "data": openReadFile(file_path, ""),
    "ct": openReadFile(file_path, ".ct"),
}

line_length = readIntFromFile(file_path, ".ll")
max_column_coord_length = readIntFromFile(file_path, ".mccl")
max_column_type_length = readIntFromFile(file_path, ".mctl")
out_col_indices = [x for x in getColIndicesToQuery(col_names_file_path, memory_map)]
out_col_coords = list(parse_data_coords(out_col_indices, file_handles["cc"], max_column_coord_length, line_length))

with open(out_file_path, 'wb') as out_file:
    # num_cols = int(len(file_handles["cc"]) / (max_column_coord_length + 1))

    all_query_col_coords = parse_data_coords(query_col_indices, file_handles["cc"], max_column_coord_length, line_length) #grabs all the coordinates from the file? Why the file handle?
    keep_row_indices = range(1, num_rows)

    for query_col_index in query_col_indices:
        keep_row_indices = filter_rows(keep_row_indices, query_col_index, [next(all_query_col_coords)]) # Get the row indexes for each column

    chunk_size = 1000
    out_lines = []

    for row_index in [0] + list(keep_row_indices):
        out_lines.append(b"\t".join(parse_data_values(row_index, line_length, out_col_coords, file_handles["data"])).rstrip()) # File handle?

        if len(out_lines) % chunk_size == 0:
            out_file.write(b"\n".join(out_lines) + b"\n")
            out_lines = []

    if len(out_lines) > 0:
        out_file.write(b"\n".join(out_lines) + b"\n")

for handle in file_handles:
    file_handles[handle].close()
