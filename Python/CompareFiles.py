from sys import argv

name, file1, file2, if_line = argv

first_file_list = []
with open(file1, "r") as first_file:
    for line in first_file:
        first_file_list.append(line)

second_file_list = []
with open(file2, "r") as second_file:
    for line in second_file:
        second_file_list.append(line)


dif = []
was_bad = False
for i, obj in enumerate(first_file_list):
    try:
        if first_file_list[i] == second_file_list[i]:
            continue
        elif first_file_list[i] != second_file_list[i]:
            sub_list = [first_file_list[i], second_file_list[i]]
            dif.append(sub_list)
    except IndexError:
        was_bad = True




if len(dif) == 0 and not was_bad:
    print(file1 + " and " + file2 + " are identical.")

else:
    if if_line == "no_line" and not was_bad:
        print("Files Not Identical")
    elif was_bad:
        print("The files have a different length (IndexError was raised while comparing files)")
    elif if_line == "yes_line":
        print("Files not identical.")
        print("Differences:")
        for i, obj in enumerate(dif):
            print("Line " + str(i + 1) + ":")
            print(obj[0])
            print(obj[1])
    else:
        print("Invalid arguement for 'if_line.' Expected 'no_line' or 'yes_line'")




