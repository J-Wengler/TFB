import sys

type_of_file = sys.argv[1]
file_path = sys.argv[2]

time = 0

with open(file_path) as input:
    for line in input:
        l_line = line.split()
        if l_line[0] == "real":
            time = l_line[1]

time_list = time.split("m")
minutes = time_list[0]
seconds = time_list[1]

seconds = seconds[:-1]

print("The " + type_of_file + " file took " + minutes + " minute(s) and " + seconds + " second(s).")

