import os

RAYLIB_CHANGED_FILES = [
    "raylib.h",
    "raudio.c",
    "raymath.h",
    "rcamera.h",
    "rcore.c",
    "rgestures.h",
    "rlgl.h",
    "rglfw.c",
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
    "void *",
    "int",
    "int*",
    "int *",
    "unsigned int",
    "unsigned int*",
    "unsigned int *",
    "char",
    "char*",
    "char *",
    "unsigned char",
    "unsigned char*",
    "unsigned char *",
    "bool",
    "bool*",
    "bool *",
    "float",
    "float*",
    "float *",
    "unsigned float",
    "unsigned float*",
    "unsigned float *",
    "double",
    "double*",
    "double *",
    "unsigned double",
    "unsigned double*",
    "unsigned double *",
    "size_t",
    "size_t*",
    "size_t *",
    "short",
    "short*",
    "short *",
    "unsigned short",
    "unsigned short*",
    "unsigned short *",
    "short int",
    "short int*",
    "short int *",
    "unsigned short int",
    "unsigned short int*",
    "unsigned short int *",
    "long",
    "long*",
    "long *",
    "unsigned long",
    "unsigned long*",
    "unsigned long *",
    "long int",
    "long int*",
    "long int *",
    "unsigned long int",
    "unsigned long int*",
    "unsigned long int *",
    "FILE",
    "FILE*",
    "FILE *",
    "float3",
    "float16"
]

c_nested_macros = [
    "#if",
    "#ifdef",
    "#ifndef"
]

kept_namespaces = [
    "android_",
    "ma_"
]

kept_typedefs = []

global raylib_typedefs
raylib_typedefs = []

global raylib_names
raylib_names = []

global raylib_typedef_names
raylib_typedef_names = []

global found_typedef
found_typedef: bool = False
global block_comment
block_comment: bool = False
global if_nesting
if_nesting: int = 0
global in_check
in_check: bool = False
global typedef_nesting
typedef_nesting = 0

g_string = 0

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
    global block_comment
    global if_nesting
    global in_check
    split_line: list = line.split(" ")
    for i in range(len(split_line)):
        # Checking if a line begins with a C macros
        if split_line[i].strip() != '' and split_line[i].strip().startswith("#"):
            # Dropping the line, as it's definitely not what we want
            return "-1"
        # Just denying whatever we find until we're outside of the block comment
        if block_comment:
            if find_comment(split_line[i], 1):
                block_comment = False
                continue
            else:
                return "-1"
        if in_check:
            in_check = False # Ensuring the check is performed only once
            # Checking if a string element contains a left parenthesis, doesn't start with asterisk/right parenthesis, and the name without it isn't already namespaced
            if not split_line[i].startswith(")") and not split_line[i].startswith("*)") \
            and "(" in list(split_line[i]) \
            and not detect_substring_in_list(split_line[i].split("(")[0], kept_namespaces):
                # If not, then we've found a function name! Returning it with stripped '*'
                return split_line[i].split("(")[0].strip("*")
            else:
                return "-1"
        # If we've found a block comment sequence, entering the mode where we deny anything until we're outside of the block comment
        if find_comment(split_line[i], 0):
            block_comment = True
            return "-1"
        # If we're inside a comment, we just deny
        if find_comment(split_line[i], 2):
            return "-1"
        if split_line[0] != "typedef" and (split_line[i] in raylib_kept_types or split_line[i] in raylib_typedefs):
            # If we've found what seems to be a type, we enable checking mode and iterate to the next element
            in_check = True
            continue
    return "-1"

def pick_typedefs(line: str) -> None:
    global found_typedef
    global typedef_nesting
    split_line = line.split(" ")
    # Deleting single-line comment from split line
    el = 0
    line_len = len(split_line)
    comm_found = False
    while el < line_len:
        # Removing empty elements in place
        if not split_line[el].strip():
            split_line.pop(el)
            line_len -= 1
            continue
        if not comm_found:
            if split_line[el] == "//":
                comm_found = True
                split_line.pop(el)
                line_len -= 1
            else:
                el += 1
        else:
            split_line.pop(el)
            line_len -= 1
    # Looking for typedefs
    for i in range(len(split_line)):
        if found_typedef:
            if split_line[i].rstrip() == "{":
                typedef_nesting += 1
            if not split_line[-1].strip().rstrip(";") in raylib_kept_types \
            and split_line[-1].strip().endswith(";") and not split_line[-1].strip().endswith(");") \
            and len(split_line) >= 3 and split_line[-2].strip() in raylib_typedefs \
            and split_line[-3].strip() == "typedef":
                raylib_typedefs.append(split_line[-1].strip().rstrip(";"))
            if split_line[-1].strip().endswith(");"):
                lsl = split_line[i].strip()
                bracket_pos = int()
                if lsl.startswith("("):
                    bracket_pos = lsl.find("(") + 1
                elif lsl.startswith("(*"):
                    bracket_pos = lsl.find("(*") + 2
                elif lsl.startswith("*(*"):
                    bracket_pos = lsl.find("*(*") + 3
                else:
                    continue
                tname = str()
                if not ")" in lsl:
                    tname = split_line[i + 1].strip(")")
                else:
                    while "".join(lsl[bracket_pos:bracket_pos + 2]).replace(" ", "") != ")(":
                        tname += lsl[bracket_pos]
                        bracket_pos += 1
                raylib_typedef_names.append(tname.lstrip("*"))
                found_typedef = False
                break
            # If we found a typedef, we're waiting for the closing bracket to get the type name after it
            if split_line[i] == "}":
                if typedef_nesting > 0:
                    typedef_nesting -= 1
                    if typedef_nesting == 0:
                        found_typedef = False
                        if split_line[i + 1].strip()[-1] == ",": # Handling typedef having two names
                            entry_one = split_line[i + 1].strip().rstrip(",")
                            entry_two = split_line[i + 2].strip().rstrip(";")
                            if not entry_one in raylib_kept_types:
                                raylib_typedefs.append(entry_one)
                            if not entry_two in raylib_kept_types:
                                raylib_typedefs.append(entry_two)
                        else:
                            entry = split_line[i + 1].strip().rstrip(";")
                            if not entry in raylib_kept_types:
                                raylib_typedefs.append(entry)
        else:
            if split_line[i] == "typedef":
                found_typedef = True

def do_namespacing(file_list: list=RAYLIB_CHANGED_FILES) -> None:
    global raylib_typedefs
    global raylib_names
    global g_string
    for each in file_list:
        yield each
        with open(each, "r") as f:
            g_string = 1
            out = list()
            lines = f.readlines()
            for line in lines:
                pick_typedefs(line)
                g_string += 1
            raylib_typedefs = list(dict.fromkeys(raylib_typedefs))
            g_string = 1
            for line in lines:
                result = check_fn_pattern(line)
                if result != "-1":
                    raylib_names.append(result)
                g_string += 1
            raylib_names = list(dict.fromkeys(raylib_names))
            for line in lines:
                new_line = line
                for a_name in raylib_names:
                    if not a_name: continue # TODO: refactor this to proper preventing of empty names at all
                    new_line = new_line.replace(f" {a_name}(", f" rl_{a_name}(")
                    new_line = new_line.replace(f"*{a_name}(", f"*rl_{a_name}(")
                    new_line = new_line.replace(f"({a_name}(", f"(rl_{a_name}(")
                    new_line = new_line.replace(f"*(*{a_name})(", f"*(*rl_{a_name})(")
                for a_type in raylib_typedefs:
                    new_line = new_line.replace(f" {a_type} ", f" Rl_{a_type} ")
                    new_line = new_line.replace(f" {a_type}*", f" Rl_{a_type}*")
                    new_line = new_line.replace(f"({a_type} ", f"(Rl_{a_type} ")
                    new_line = new_line.replace(f"({a_type}*", f"(Rl_{a_type}*")
                    new_line = new_line.replace(f"({a_type})", f"(Rl_{a_type})")
                    new_line = new_line.replace(f"(*{a_type})(", f"(*Rl_{a_type})(")
                    new_line = new_line.replace(f" {a_type};", f" Rl_{a_type};")
                    new_line = new_line.replace(f" {a_type},", f" Rl_{a_type},")
                    new_line = new_line.replace(f",{a_type};", f",Rl_{a_type};")
                for a_name in raylib_typedef_names:
                    if not a_name: continue # TODO: refactor this to proper preventing of empty names at all
                    new_line = new_line.replace(f"({a_name})(", f"(rl_{a_name})(")
                    new_line = new_line.replace(f"(*{a_name})(", f"(*rl_{a_name})(")
                    new_line = new_line.replace(f"({a_name} ", f"(rl_{a_name} ")
                    new_line = new_line.replace(f" {a_name} ", f" rl_{a_name} ")
                out.append(new_line)
        with open(each, "w") as f:
            for i in out:
                f.write(i)
