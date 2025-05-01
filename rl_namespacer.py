import os

RAYLIB_CHANGED_FILES = [
    "raylib.h",
    "raudio.c",
    "raymath.h",
    "rcamera.h",
    "rcore.c",
    "rgestures.h",
    "rglfw.c",
    #"rlgl.h", # Excluded deliberately
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

c_nested_macros = [
    "#if",
    "#ifdef",
    "#ifndef"
]

kept_namespaces = [
    "android_",
    "rl",
    "ma_"
]

global raylib_typedefs
raylib_typedefs = []

RAYLIB_PATH = "deps/raylib/src"

global raylib_names
raylib_names = []

global found_typedef
found_typedef: bool = False
global block_comment
block_comment: bool = False
global if_nesting
if_nesting: int = 0
global in_check
in_check: bool = False

g_string = 0

DEBUG = True

def debug_print(s: str) -> None:
    if DEBUG:
        print(s)

### Finding a comment in a line (single or multiline) 
def find_comment(s: str, oc: int) -> bool:
    # oc = 0 => multiline comment begins
    # oc = 1 => multiline comment ends
    # oc = 2 => single-line comment
    if len(s) >= 2:
        cs = str()
        if oc == 0: cs = "/*"
        elif oc == 1: cs = "*/"
        elif oc == 2: cs = "//"
        for i in range(len(s)):
            if s[i:i+2] == cs:
                return True
    return False

### Detects if a function already belongs to a namespace
def detect_substring_in_list(s: str, l: list) -> bool:
    for each in l:
        if "".join(list(s)[0:len(each)]) == each:
            return True
    return False

### Checking if a line contains token pattern of a C function
### Return codes:
### "-1" - No function name pattern found
### anything else - Function name
def check_fn_pattern(line: str) -> str:
    # [static ][unsigned ]<type ><FuncName>(
    global block_comment
    global if_nesting
    global in_check
    split_line: list = line.split(" ")
    for i in range(len(split_line)):
        # Checking if macro-nesting level is higher than 0 (and thus, present)
        if if_nesting > 0:
            # Decreasing nesting level upon meeting "#endif"
            if split_line[i].strip() != '' and "".join(list(split_line[i].strip())[0:6]) == "#endif":
                if_nesting -=1
            # Dropping the line anywise, as even with nesting reaching zero we don't need it
            return "-1"
        # Checking if line begins with a C macros
        if split_line[i].strip() != '' and list(split_line[i].strip())[0] == "#":
            if detect_substring_in_list(split_line[i], c_nested_macros): # If the line contains an if-macro, increasing nesting level
                if_nesting += 1
            # Dropping the line, as it's definitely not what we want
            return "-1"
        if block_comment:
            if find_comment(split_line[i], 1):
                block_comment = False
                continue
            else:
                return "-1"
        if in_check:
            in_check = False
            if "(" in list(split_line[i]) and not detect_substring_in_list(split_line[i].split("(")[0], kept_namespaces): #and split_line[i].split("(")[0].strip("!") != "defined":
                return split_line[i].split("(")[0]
            else:
                return "-1"
        if find_comment(split_line[i], 0):
            block_comment = True
            return "-1"
        if find_comment(split_line[i], 2):
            return "-1"
        if split_line[0] != "typedef" and (split_line[i] in raylib_kept_types or split_line[i] in raylib_typedefs):
            #debug_print(str(g_string) + "t " + split_line[i])
            in_check = True
            continue
    return "-1"

def pick_typedefs(line: str) -> None:
    global found_typedef
    split_line = line.split(" ")
    # Looking for a single-line comment to purge it from the line
    comm_purge = False
    purge_indexes = []
    for i in range(len(split_line)):
        if find_comment(split_line[i], 2):
            comm_purge = True
        if comm_purge:
            purge_indexes.append(i)
    for i in purge_indexes:
        split_line.pop(i)
    # Looking for typedefs
    for i in range(len(split_line)):
        if found_typedef:
            if "".join(list(split_line[-1].strip())[-2:]) == ");":
                debug_print(str(g_string) + " " + "".join(list(split_line[-1].strip())[-2:]))
                lsl = list(split_line[-1].strip())
                bracket_pos = int()
                for symbol in range(len(lsl)):
                    if "".join(lsl[symbol:symbol + 2]) == "(*":
                        bracket_pos = symbol + 3
                tname = str()
                while "".join(lsl[bracket_pos:bracket_pos + 2]) != ")(":
                    tname += lsl[bracket_pos]
                    bracket_pos += 1
                debug_print(tname)
                raylib_typedefs.append(tname)
                break
            if split_line[i] == "}":
                found_typedef = False
                if split_line[i + 1].strip()[-1] == ",":
                    entry_one = split_line[i + 1].strip().strip(",")
                    entry_two = raylib_typedefs.append(split_line[i + 2].strip().strip(";"))
                    if not entry_one in raylib_kept_types:
                        raylib_typedefs.append(entry_one)
                    if not entry_two in raylib_kept_types:
                        raylib_typedefs.append(entry_two)
                else:
                    entry = split_line[i + 1].strip().strip(";")
                    if not entry in raylib_kept_types:
                        raylib_typedefs.append(entry)
            else: break
        else:
            if split_line[i] == "typedef":
                found_typedef = True

for each in RAYLIB_CHANGED_FILES:
    out = list()
    debug_print(each)
    with open(each, "r") as f:
        g_string = 1
        lines = f.readlines()
        for line in lines:
            pick_typedefs(line)
            raylib_typedefs = list(dict.fromkeys(raylib_typedefs))
            g_string += 1
        g_string = 1
        for line in lines:
            result = check_fn_pattern(line)
            if result != "-1":
                raylib_names.append(result)
                #debug_print(str(g_string) + "f " + str(result))
            g_string += 1
        #for line in lines:
        #    for each in raylib_typedefs:
        #        debug_print(each)
                #out.append(line.replace(each, "rl" + each))
        #    for each in raylib_names:
        #        debug_print(each)
                #out.append(line.replace(each, "rl" + each))
    #with open(each, "w") as f:
    #    f.write(out)
debug_print(raylib_typedefs)
#debug_print(raylib_names)
