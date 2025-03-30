import os

RAYLIB_CHANGED_FILES = [
    "raylib.h",
    "raudio.c",
    "raymath.h",
    "rcamera.h",
    "rcore.c",
    "rgestures.h",
    "rglfw.c",
    #"rlgl.h",
    "rmodels.c",
    "rshapes.c",
    "rtext.c",
    "rtextures.c",
    "utils.c",
    "utils.h"
]

raylib_kept_types = [
    "void",
    "void*",
    "int",
    "int*",
    "char",
    "char*",
    "bool",
    "bool*",
    "float",
    "float*",
    "double",
    "double*",
    "size_t",
    "size_t*",
    "short",
    "short*",
    "short int",
    "short int*",
    "long",
    "long*",
    "long int",
    "long int*",
    "FILE",
    "FILE*"
]

global raylib_typedefs
raylib_typedefs = []

RAYLIB_PATH = "deps/raylib/src"

global raylib_names
raylib_names = []

global found_typedef
found_typedef = False
global block_comment
block_comment = False
global in_check
in_check = False

g_string = 0

### Checking if a line contains token pattern of a C function
### Return codes:
### "-1" - No function name pattern found
### anything else - Function name
def check_fn_pattern(line: str) -> str:
    # [static ][unsigned ]<type ><FuncName>(
    global block_comment
    global in_check
    split_line = line.split(" ")
    for i in split_line:
        if in_check:
            if "(" in list(i):
                return i.split("(")[0]
        if block_comment:
            if i == "*/":
                block_comment = False
                continue
            else:
                return "-1"
        if i == "/*":
            block_comment = True
            return "-1"
        if i == "//":
            return "-1"
        if i in raylib_kept_types or i in raylib_typedefs:
            in_check = True
            continue

def pick_typedefs(line: str) -> None:
    global found_typedef
    split_line = line.split(" ")
    # Looking for typedefs
    for i in range(len(split_line)):
        if found_typedef:
            if split_line[i] == "}":
                found_typedef = False
                if split_line[i + 1].strip()[-1] == ",":
                    raylib_typedefs.append(split_line[i + 1].strip().strip(","))
                    print(str(g_string) + " " + split_line[i + 1].strip().strip(","))
                else:
                    raylib_typedefs.append(split_line[i + 1].strip().strip(";"))
                    print(str(g_string) + " " + split_line[i + 1].strip().strip(";"))
            break
        else:
            if split_line[i] == "typedef":
                found_typedef = True

def dedup(l: list) -> list:
    i: int = 0
    j: int = 1
    while i < len(l):
        j = i + 1
        while j < len(l):
            if l[i] == l[j]:
                l.pop(j)
            else:
                j += 1
        i += 1
    return l

for each in RAYLIB_CHANGED_FILES:
    out = list()
    print(each)
    with open(each, "r") as f:
        g_string = 1
        lines = f.readlines()
        for line in lines:
            pick_typedefs(line)
            raylib_typedefs = dedup(raylib_typedefs)
            g_string += 1
        for line in lines:
            result = check_fn_pattern(line)
            if result != "-1":
                raylib_names.append(result)
        #for line in lines:
        #    for each in raylib_typedefs:
        #        print(each)
                #out.append(line.replace(each, "rl" + each))
        #    for each in raylib_names:
        #        print(each)
                #out.append(line.replace(each, "rl" + each))
    #with open(each, "w") as f:
    #    f.write(out)
#print(raylib_typedefs)
#print(raylib_names)
