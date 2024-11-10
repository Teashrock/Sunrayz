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

raylib_valid_types = [
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

raylib_new_types = []

RAYLIB_PATH = "deps/raylib/src"

raylib_names = []

seek_rbracket = False
not_a_type = False
g_string = 0

def check_fn_pattern(s: str) -> bool:
    # [static ][unsigned ]<type ><FuncName>(
    s = s.split(" ")
    

# Finding definitions
for file in RAYLIB_CHANGED_FILES:
    with open(os.path.join(RAYLIB_PATH, file), "r") as handled_file:
        file_lines = handled_file.readlines()
        g_string = 0
        # Looking for types
        for line in file_lines:
            g_string += 1
            if seek_rbracket:
                if not line[0] == "}":
                    continue
                else:
                    i = 2
                    rec_stop = ";"
                    seek_rbracket = False
            else:
                rettype_pos = line.find("typedef ")
                if rettype_pos != 0:
                    continue
                # Checking typedef
                i = len("typedef ") + rettype_pos
                typedef_word = str()
                while line[i] != " ":
                    typedef_word += line[i]
                    i += 1
                i += 1
                if typedef_word == "enum":
                    seek_rbracket = True
                    continue
                if not typedef_word == "struct":
                    type_name = str()
                    while line[i] != ";":
                        type_name += line[i]
                        i += 1
                    raylib_new_types.append(type_name)
                    continue
                rec_stop = " "
            type_name = str()
            while (line[i] != rec_stop):
                if line[i] == "{":
                    seek_rbracket = True
                    break
                if line[i] == "[" and line[i + 1] == "]":
                    not_a_type = True
                    break
                type_name += line[i]
                i += 1
            if seek_rbracket or not_a_type:
                continue
            raylib_new_types.append(type_name)
    
        raylib_valid_types += raylib_new_types

        # Looking for names
        g_string = 0
        for line in file_lines:
            g_string += 1
            skip = False
            if line.find("typedef ") != -1: # Skipping typedefs
                continue
            for a_type in raylib_valid_types:
                retcom_pos = line.find("//")
                type_marker: str = a_type + " "
                rettype_pos = line.find(type_marker)
                if rettype_pos == -1:
                    type_marker = "static "
                    rettype_pos = line.find(type_marker)
                    if rettype_pos != -1:
                        rettype_pos += len(type_marker)
                        if not rettype_pos > retcom_pos:
                            while line[rettype_pos] != " ":
                                rettype_pos += 1
                if (retcom_pos != -1 and rettype_pos > retcom_pos) or rettype_pos == -1:
                    break
                print(type_marker)
                i = len(type_marker) + rettype_pos
                found_name = str()
                while i < len(line):
                    if line[i] == "(":
                        break
                    elif line[i] == " ":
                        found_name = ""
                        break
                    else:
                        found_name += line[i]
                    i += 1
                if found_name == "":
                    break
                print(file)
                print(g_string)
                print(found_name)
                if not found_name.find("android_"):
                    raylib_names.append(found_name)
                break

#print(raylib_names)