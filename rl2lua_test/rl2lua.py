def find_comment(s: str, oc: int) -> bool:
    """
    Finding a comment in a line (single or multiline)
    s => the string to scan
    oc = 0 => multiline comment begins
    oc = 1 => multiline comment ends
    oc = 2 => single-line comment
    """
    if len(s) >= 2:
        cs = str()
        if oc == 0: cs = "/*"
        elif oc == 1: cs = "*/"
        elif oc == 2: cs = "//"
        for i in range(len(s)):
            if s[i:i+2] == cs:
                return True
    return False

def detect_substring_in_list(s: str, l: list) -> bool:
    """
    Detects if a function already belongs to a namespace
    """
    for each in l:
        if "".join(list(s)[0:len(each)]) == each:
            return True
    return False

def check_fn_pattern_old(line: str) -> str:
    """
    Checking if a line contains token pattern of a C function
    Return codes:
    "-1" - No function name pattern found
    anything else - Function name
    """
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
            # Checking if a string element contains a left parenthesis, doesn't start with an asterisk/right parenthesis, and the name without it isn't already namespaced
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

def check_fn_pattern(line: str) -> tuple[str, str]:
    if line.startswith("RLAPI"):
        decl_end_position = line.find(";")
        lbracket_pos = line.find("(")
        lspace_pos = lbracket_pos
        while line[lspace_pos] != " ":
            lspace_pos -= 1
        return (line[:decl_end_position + 1].lstrip("RLAPI "), line[lspace_pos + 1:lbracket_pos])
    return ("-1", "-1")

def rl_parse(path: str = ""):
    if not path:
        path = "raylib.h"
    rl_names = set()
    rl_sheet = str()
    with open(path, "r") as raylib_h:
        for line in raylib_h.readlines():
            result = check_fn_pattern(line)
            if result != ("-1", "-1"):
                rl_names.add(result[1])
                rl_sheet += (result[0] + "\n")
        with open("raylib.lua.c", "w") as rl_lua_c:
            rl_lua_c.write("#include <lua.h>\n#include <lauxlib.h>\n\n")
            for func_name in rl_names:
                rl_lua_c.write("static int lua_{}(lua_State* L) ".format(func_name))
                rl_lua_c.write(r"{")
                rl_lua_c.write("\n\t")
            rl_lua_c.write("int luaopen_raylib(lua_State* L) {\n")
            rl_lua_c.write("}\n")

rl_parse("raylib.h")