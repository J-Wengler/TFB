import os
import sys
from Helper import *
from fastnumbers import *

file_path = sys.argv[1]
out_file_path = sys.argv[2]

column_size_dict = {}
column_start_coords = []
column_types_dict = {}

def get_type(a_string):
    return ("s", "n")[isfloat(a_string)] #string or number

def get_most_generic_type(types_set):
    if len(types_set) > 1:
        return "s" # string

    return list(types_set)[0]

# Initialize a dictionary with the column index as key and width of the column as value
with open(file_path, 'rb') as my_file:
    header_items = my_file.readline().rstrip(b"\n").split(b"\t")

    for i in range(len(header_items)):
        column_size_dict[i] = 0

# Iterate through the lines to find the max width and type of each column
with open(file_path, 'rb') as my_file:
    for line in my_file:
        line_items = line.rstrip(b"\n").split(b"\t")

        for i in range(len(line_items)):
            column_size_dict[i] = max([column_size_dict[i], len(line_items[i])])

            if not i in column_types_dict:
                # This is the header line, so we don't need to check its type
                column_types_dict[i] = set()
            else:
                column_types_dict[i].add(get_type(line_items[i]))

# Calculate the length of the first line (and thus all the other lines)
line_length = sum([column_size_dict[i] for i in range(len(header_items))])

# Save value that indicates line length
with open(out_file_path + ".ll", 'wb') as ll_file:
    ll_file.write(str(line_length + 1).encode())

# Calculate the positions where each column starts
cumulative_position = 0
for i in range(len(header_items)):
    column_size = column_size_dict[i]
    column_start_coords.append(str(cumulative_position))
    cumulative_position += column_size
column_start_coords.append(str(line_length + 1))
# Calculate the column coordinates and max length of these coordinates
column_coords_string, max_column_coord_length = buildStringMap(column_start_coords)

# Save column coordinates
with open(out_file_path + ".cc", 'wb') as cc_file:
    cc_file.write(column_coords_string)

# Save value that indicates maximum length of column coords string
with open(out_file_path + ".mccl", 'wb') as mccl_file:
    mccl_file.write(max_column_coord_length)

# Find most generic data type for each column
column_types = []
for i, types in sorted(column_types_dict.items()):
    column_types.append(get_most_generic_type(types))

# Calculate the column types and max length of these types
column_types_string, max_column_types_length = buildStringMap(column_types)

# Save column types
with open(out_file_path + ".ct", 'wb') as ct_file:
    ct_file.write(column_types_string)

# Save value that indicates maximum length of column types
with open(out_file_path + ".mctl", 'wb') as mctl_file:
    mctl_file.write(max_column_types_length)

# Save the data to output file
with open(file_path, 'rb') as my_file:
    with open(out_file_path, 'wb') as out_file:
        line_number = 0
        cumulative_position = 0

        for line in my_file:
            line_items = line.rstrip(b"\n").split(b"\t")

            line_out = ""
            for i in sorted(column_size_dict.keys()):
                format_string = "{:<" + str(column_size_dict[i]) + "}"
                column_value = format_string.format(line_items[i].decode())
                line_out += column_value

            # This newline character is unnecessary, so it adds a bit of disk space.
            # However, it makes the files much more readable to humans.
            line_out = (line_out + "\n").encode()
            out_file.write(line_out)

            line_number += 1
            cumulative_position += len(line_out)
