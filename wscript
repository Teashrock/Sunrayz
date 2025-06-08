VERSION    = "master"
APPNAME    = "sunrayz"
REVISION   = ""
BUILD_TYPE = "debug"
DYNAMIC_LINKING = True

top = '.'
out = "build"

import glob
import os
import platform
import shutil
import subprocess
import sys
import json
from font2code import font_transform
from rl_namespacer import do_namespacing
from waflib.Tools.compiler_c import c_compiler
from waflib import Logs
c_compiler['win32'] = ['gcc']

RAYLIB_REPO    = "https://github.com/raysan5/raylib"
RAYLIB_VERSION = "5.0"

RAYGUI_REPO    = "https://github.com/raysan5/raygui"
RAYGUI_VERSION = "4.0"

KUROKO_REPO    = "https://github.com/kuroko-lang/kuroko"
KUROKO_VERSION = "1.4.0"

LUAJIT_REPO    = "https://github.com/LuaJIT/LuaJIT"
LUAJIT_VERSION = "2.0.5"

GETTEXT_REPO    = "https://ftp.gnu.org"
GETTEXT_VERSION = "0.21.1"

BUILD_DIR = os.getcwd()
CACHE_DIR = BUILD_DIR + "/cache"
SRC_DIR   = os.path.join(BUILD_DIR, "src")

global EXE_NAME
EXE_NAME = "sunrayz"
global DEBUGGER
DEBUGGER = str()

if platform.system() == "Windows":
    MAKE_COMMAND = "mingw32-make"
else:
    MAKE_COMMAND = "make"
MAKE_ARGUMENTS = "-j9"

INSTALLPATH = os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE)

# 0 — general utility
# 1 — Windows-specific utility
# 2 — general utility needed if master git branches are used
UTIL_LIST = {
    "unix2dos": 1,
    "patch": 0,
    "gcc": 0,
    "git": 2
}

global patch_list
patch_list : list = list()

global download_only
download_only : bool = False

def options(opt):
    if platform.system() == "Windows":
        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\x86_64-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    opt.load('compiler_c')

def configure(conf):
    delim : str = ""
    ext : str = ""
    if not download_only:
        if platform.system() == "Windows":
            delim = ";"
            ext = ".exe"
        else:
            delim = ":"
        util_check_paths : list = os.getenv("PATH").split(delim)
        for key in UTIL_LIST.keys():
            found : bool = False
            if UTIL_LIST[key] == 0 or \
                (UTIL_LIST[key] == 1 and platform.system() == "Windows") or \
                (UTIL_LIST[key] == 2 and (RAYLIB_VERSION == "master" or \
                    RAYGUI_VERSION == "master" or \
                    KUROKO_VERSION == "master")):
                for each in util_check_paths:
                    if os.path.exists(os.path.join(each, key + ext)):
                        if key == "gcc":
                            with open("compiler.path", "w") as cp:
                                cp.write(each + '\n')
                        found = True
                        break
                if not found:
                    Logs.error("Error: \"" + key + "\" utility has not been found in PATH.")
                    exit(1)

    clist : list = []
    def download(what : str, version : str, repo : str):
        try:
            clistfile = open("downloaded.list", "r")
            clist = clistfile.readlines()
            clistfile.close()
        except:
            clist = []
        if not what in clist:
            if version == "master":
                sp = subprocess.Popen(["git", "clone", "--recursive", repo, os.path.join(CACHE_DIR, what)])
                sp.wait()
            else:
                if not os.path.exists(CACHE_DIR):
                    os.mkdir(CACHE_DIR)
                tar_path_name = os.path.join(CACHE_DIR, ("{}-{}.tar.gz".format(what, version)))
                if not os.path.exists(os.path.join(os.path.dirname(tar_path_name), tar_path_name.split(os.path.sep)[-1].split('.')[0].split('-')[0])):
                    if not os.path.exists(tar_path_name):
                        with open(tar_path_name, "wb") as tarar:
                            if repo.split("://")[1].split('/')[0] == "github.com":
                                Logs.warn("Downloading {}-{}.tar.gz, this may take a while...".format(what, version))
                                try:
                                    import requests
                                    cnt = requests.get(repo + ("/archive/refs/tags/%s.tar.gz" % version), allow_redirects=True).content
                                    if cnt == b"404: Not Found":
                                        cnt = requests.get(repo + ("/archive/refs/tags/v%s.tar.gz" % version), allow_redirects=True).content
                                except ModuleNotFoundError:
                                    import urllib.request
                                    try:
                                        cnt = urllib.request.urlopen(repo + ("/archive/refs/tags/%s.tar.gz") % version).read()
                                    except urllib.error.HTTPError:
                                        cnt = urllib.request.urlopen(repo + ("/archive/refs/tags/v%s.tar.gz") % version).read()
                            elif repo.split("://")[1].split('/')[0] == "ftp.gnu.org" and repo == GETTEXT_REPO:
                                Logs.warn("Downloading {}-{}.tar.gz, this may take a while...".format(what, version))
                                try:
                                    import requests
                                    cnt = requests.get(repo + ("/pub/gnu/{}/{}-{}.tar.gz").format(what, what, version), allow_redirects=True).content
                                    if cnt == b"404: Not Found":
                                        pass
                                except ModuleNotFoundError:
                                    import urllib.request
                                    try:
                                        cnt = urllib.request.urlopen(repo + ("/pub/gnu/{}/{}-{}.tar.gz").format(what, what, version)).read()
                                    except urllib.error.HTTPError:
                                        pass
                            tarar.write(cnt)
                    if not os.path.exists(os.path.join(DEPS_DIR, what)):
                        import tarfile
                        tf = tarfile.open(tar_path_name, 'r')
                        Logs.warn(("Extracting {}-{}.tar.gz...").format(what, version))
                        tf.extractall(DEPS_DIR)
                        shutil.move(os.path.join(DEPS_DIR, "{}-{}".format(what, version)), os.path.join(DEPS_DIR, what))
                Logs.info("Done!")
            try:
                clistfile = open("downloaded.list", "r")
                clistfile_dump = clistfile.read()
            except:
                clistfile_dump = ""
            clistfile = open("downloaded.list", "w")
            clistfile.write(clistfile_dump)
            clistfile.write(what + "\n")
            clistfile.close()

    if not download_only:
        if platform.system() == "Windows":
	        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\x86_64-w64-mingw32\\8.1.0;" + os.environ["PATH"]
        conf.load('compiler_c')
        conf.env.THIS = os.getcwd()
        DEPS_DIR = os.path.join(conf.env.THIS, "deps")
        Logs.warn("Configuring...")
    if download_only:
        Logs.warn("Downloading...")
    if not os.path.exists(DEPS_DIR): os.mkdir(DEPS_DIR)
    download("raylib", RAYLIB_VERSION, RAYLIB_REPO)
    download("raygui", RAYGUI_VERSION, RAYGUI_REPO)
    download("kuroko", KUROKO_VERSION, KUROKO_REPO)
    download("LuaJIT", LUAJIT_VERSION, LUAJIT_REPO)
    #download("gettext", GETTEXT_VERSION, GETTEXT_REPO)

    if download_only:
        exit()
    
    # Patch zone
    for _, _, f in os.walk(os.path.join(BUILD_DIR, "patches")):
        for i in f:
            shutil.copy(os.path.join(BUILD_DIR, "patches", i), os.path.join(BUILD_DIR, out))
            patch_list.append(i)
    if platform.system() == "Windows":
        Logs.warn("Converting patches' ends-of-line to CR LF, necessary on Windows...")
        for i in patch_list:
            sp = subprocess.Popen(["unix2dos", os.path.join(BUILD_DIR, out, i)])
            sp.wait()
        Logs.info("Done!")

    # Raylib and Raygui patches
    Logs.warn("Applying architecture patches for Raylib and Raygui...")
    os.chdir("deps/raylib/src")
    sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "raylib_utils.patch")])
    sp.wait()
    sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "raylib_raylib.patch")])
    sp.wait()
    os.chdir(BUILD_DIR + "/deps/raygui/src")
    sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "raygui.patch")])
    sp.wait()
    
    # Dividing raygui.h into two separate parts
    p : str = ""
    raygui_code : str = ""
    with open("raygui.h", "r") as raygui_h:
        p = raygui_h.read().replace("#if defined(RAYGUI_IMPLEMENTATION)", "#if 0 // WAF Checkpoint", 1)
        raygui_h.seek(0, 0)
        rg_line_found : bool = False
        for line in raygui_h.readlines():
            if rg_line_found:
                if line.strip() == "#endif      // RAYGUI_IMPLEMENTATION":
                    break
                else:
                    raygui_code += line
            if line.strip() == "#if defined(RAYGUI_IMPLEMENTATION)":
                rg_line_found = True
    with open("raygui.h", "w") as raygui_h:
        raygui_h.write(p)
    os.chdir(BUILD_DIR)
    if not os.path.exists(BUILD_DIR + "/src/external"):
        os.mkdir(BUILD_DIR + "/src/external")
    with open(BUILD_DIR + "/src/external/raygui.c", "w") as raygui_c:
        raygui_c.write("#include <raygui.h>\n")
        raygui_c.write(raygui_code)
    
    p = ""
    Logs.warn("Changing Raylib build type to shared... ")
    if DYNAMIC_LINKING:
        with open("deps/raylib/src/Makefile", "r") as rf:
            p = rf.read().replace("RAYLIB_LIBTYPE       ?= STATIC", "RAYLIB_LIBTYPE       ?= SHARED", 1)
    
        with open("deps/raylib/src/Makefile", "w") as rf:
            rf.write(p)
    if BUILD_TYPE == "debug":
        with open("deps/raylib/src/Makefile", "r") as rf:
            p = rf.read().replace("RAYLIB_BUILD_MODE    ?= RELEASE", "RAYLIB_BUILD_MODE    ?= DEBUG", 1)
    
        with open("deps/raylib/src/Makefile", "w") as rf:
            rf.write(p)
    
    if platform.system() == "Linux":
        Logs.warn("Patching Raylib Makefile...")
        os.chdir("deps/raylib/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "raylib_makefile_linux.patch")])
        sp.wait()
        os.chdir(BUILD_DIR)
    
    if platform.system() == "Haiku":
        Logs.warn("Patching Raylib Makefile...")
        os.chdir("deps/raylib/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "raylib_makefile_haiku.patch")])
        sp.wait()
        os.chdir(BUILD_DIR)

        Logs.warn("Applying special Raylib patch for Haiku OS...")
        os.chdir("deps/raylib")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "raylib_haiku.patch")])
        sp.wait()
        os.chdir(BUILD_DIR)

    os.chdir(os.path.join(BUILD_DIR, "deps", "raylib", "src"))    
    Logs.warn("Prefixing all Raylib types and functions to ensure namespacing...")
    do_namespacing()
    os.chdir(BUILD_DIR)
    
    Logs.info("All done here!")

    # Kuroko patches
    if platform.system() == "Windows" and not "kuroko" in clist:
        # This is necessary on MinGW
        Logs.warn("Preparing Kuroko Makefile...")
        os.chdir("deps/kuroko")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "kuroko_makefile_windows.patch")])
        sp.wait()
        Logs.info("Done!")
        Logs.warn("Checking if GCC can floor without libm...")
        with open("floor_check.c", "w") as test_c:
            test_c.write("""#include <stdio.h>
#include <stdlib.h>
int main(int argc, char * argv[]) {
	return printf("%f", __builtin_floor(strtod(argv[1],NULL)));
}""")
        floor_check = subprocess.run(["gcc", "-o", "NUL", "-x", "c", "floor_check.c"])
        if floor_check.returncode != 0 or floor_check.stderr != None:
            Logs.error("False!")
            Logs.warn("Current GCC can't floor without libm, patching Kuroko Makefile...")
            with open("Makefile", "r") as kf:
                p = kf.read().replace('SUNRAYZ_FLOOR_WITHOUT_LIBM_CHECK', 'LDLIBS += -lm', 1)
            with open("Makefile", "w") as kf:
                kf.write(p)
        else:
            Logs.info("Success!")
            Logs.warn("Current GCC can floor by itself, clearing Kuroko Makefile...")
            with open("Makefile", "r") as kf:
                p = kf.read().replace('SUNRAYZ_FLOOR_WITHOUT_LIBM_CHECK', '', 1)
            with open("Makefile", "w") as kf:
                kf.write(p)
        Logs.info("Done!")
        os.chdir(BUILD_DIR)
        #Logs.warn("Patching Kuroko rline.c... ")
        #with open("deps/kuroko/src/vendor/rline.c", "r") as kf:
        #    p = kf.read().replace('#include "wcwidth._h"', "#include \"wcwidth._h\"\n#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200\n#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004", 1)
        #with open("deps/kuroko/src/vendor/rline.c", "w") as kf:
        #    kf.write(p)
        Logs.warn("Patching Kuroko module_socket.c...")
        os.chdir("deps/kuroko/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "kuroko_module_socket_windows.patch")])
        sp.wait()
        Logs.info("Done!")
    elif platform.system() == "Haiku" and not "kuroko" in clist:
        Logs.warn("Patching Kuroko Makefile... ")
        os.chdir("deps/kuroko")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, out, "kuroko_makefile_haiku.patch")])
        sp.wait()
        Logs.info("Done!")
    
    Logs.warn("Now beginning to transform fonts...")
    for _, _, f in os.walk(os.path.join("assets", "fonts")):
        for each in f:
            Logs.warn("Transforming " + each + "...")
            if not os.path.exists(BUILD_DIR + "/src/assets_gen/fonts"):
                os.mkdir(BUILD_DIR + "/src/assets_gen/fonts")
            font_transform(each)
    Logs.info("Done!")

def download(ctx):
    global download_only
    download_only = True
    configure(ctx)

def purge(ctx):
    try:
        shutil.rmtree(os.path.join(BUILD_DIR, "build"))
    except: pass
    try:
        shutil.rmtree(os.path.join(BUILD_DIR, "deps"))
    except: pass
    try:
        shutil.rmtree(os.path.join(BUILD_DIR, "result"))
    except: pass
    try:
        os.remove(os.path.join(BUILD_DIR, ".lock-waf_" + sys.platform + "_build"))
    except: pass
    try:
        os.remove(os.path.join(BUILD_DIR, "compiled.list"))
    except: pass
    try:
        os.remove(os.path.join(BUILD_DIR, "compiler.path"))
    except: pass
    try:
        os.remove(os.path.join(BUILD_DIR, "downloaded.list"))
    except: pass
    try:
        os.remove(os.path.join(BUILD_DIR, "fonts_linked.list"))
    except: pass
    for _, _, f in os.walk(os.path.join(BUILD_DIR, "src", "assets_gen", "fonts")):
        for each in f:
            if each.split('.')[-1] == 'c':
                os.remove(os.path.join(BUILD_DIR, "src", "assets_gen", "fonts", each))
    try:
        os.remove(os.path.join(BUILD_DIR, "src", "external", "raygui.c"))
    except: pass
    
    #gitignore_dump = str()
    #with open(".gitignore", "r") as gitignore:
    #    gitignore_dump = gitignore.readlines()
    #with open(".gitignore", "w") as gitignore:
    #    for each in gitignore_dump:
    #        if not each.startswith("src/assets_gen/fonts/"):
    #            gitignore.write(each)

    if not os.path.exists(os.path.join("src", "assets_gen", "fonts")):
        os.mkdir(os.path.join("src", "assets_gen", "fonts"))

    with open(os.path.join("src", "assets_gen", "fonts", "fonts.h"), "w") as fonts_h:
        fonts_h.write("#pragma once\n\n// This file only links static fonts with the engine.\n// Edit it only if you know what you're doing.\n")

def distclean(ctx):
    purge(ctx)

def uncache(ctx):
    shutil.rmtree(os.path.join(BUILD_DIR, "cache"))

def build(ctx):
    os.chdir(BUILD_DIR)
    if platform.system() == "Windows":
        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\x86_64-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    DEPS_DIR = os.path.join(os.getcwd(), "deps")
    if ctx.cmd == "clean":
        if platform.system() == "Windows":
            os.environ["CC"] = "x86_64-w64-mingw32-gcc"
        os.chdir(os.path.join(DEPS_DIR, "kuroko"))
        sp = subprocess.Popen([MAKE_COMMAND, "clean"])
        sp.wait()
        os.chdir(os.path.join(DEPS_DIR, "raylib", "src"))
        sp = subprocess.Popen([MAKE_COMMAND, "clean"])
        sp.wait()
        shutil.rmtree(os.path.join(BUILD_DIR, "result"))
    else:
        try:
            clistfile = open("compiled.list", "r")
            clist = clistfile.read().split('\n')
        except:
            clistfile = open("compiled.list", "w")
            clist = []
        MODE = ""
        OPT = ""
        if BUILD_TYPE  == "debug":
            MODE = "-g"
            OPT  = "-O0"
        else:
            MODE = "-s"
            OPT  = "-O1"
        if not os.path.exists(os.path.join(os.getcwd(), "result", platform.system() + "-" + BUILD_TYPE)):
            os.makedirs(os.path.join(os.getcwd(), "result", platform.system() + "-" + BUILD_TYPE))
        
        krklib_name = ""
        if platform.system() == "Windows":
            krklib_name = "libkuroko.dll"
        else:
            krklib_name = "libkuroko.so"
        if not "kuroko" in clist:
            if not os.path.exists(os.path.join(DEPS_DIR, "kuroko", krklib_name)):
                # Kuroko build
                Logs.warn("Building Kuroko...")
                # cd kuroko
                os.chdir(os.path.join(DEPS_DIR, "kuroko"))
                if platform.system() == "Windows":
                    # set CC=i686-w64-mingw32-gcc
                    os.environ["CC"] = "x86_64-w64-mingw32-gcc"
            # mingw32-make
            if platform.system() == "Haiku":
                os.environ["KRK_DISABLE_THREADS"] = "1"
            sp = subprocess.Popen([MAKE_COMMAND, MAKE_ARGUMENTS])
            sp.wait()
            os.chdir(BUILD_DIR)
            clistfile.write("kuroko\n")

        rllib_name = ""
        if platform.system() == "Windows":
            if DYNAMIC_LINKING:
                rllib_name = "raylib.dll"
            else:
                rllib_name = "raylib.a"
        else:
            if DYNAMIC_LINKING:
                rllib_name = "libraylib.so"
            else:
                rllib_name = "libraylib.a"
        if not "raylib" in clist:
            if not os.path.exists(os.path.join(DEPS_DIR, "raylib", "src", rllib_name)):
                # Raylib build
                Logs.warn("\nBuilding Raylib...")
                # cd raylib/src
                os.chdir(os.path.join(DEPS_DIR, "raylib", "src"))
                if platform.system() == "Windows":
    	    		# del /q /s raylib.rc.data raylib.dll.rc.data
                	os.remove("raylib.rc.data")
                	os.remove("raylib.dll.rc.data")
                	# windres raylib.rc -o raylib.rc.data
                	# windres raylib.dll.rc -o raylib.dll.rc.data
                	os.system("windres raylib.rc -o raylib.rc.data")
                	os.system("windres raylib.dll.rc -o raylib.dll.rc.data")
                # mingw32-make
                sp = subprocess.Popen([MAKE_COMMAND, MAKE_ARGUMENTS])
                sp.wait()
                os.chdir(BUILD_DIR)
                clistfile.write("raylib\n")

        clistfile.close()
        os.chdir(SRC_DIR)

        SRCS = []
        src_path_len = len(os.path.join(BUILD_DIR, "src").split(os.path.sep))
        for n, _, f in os.walk(os.path.join(BUILD_DIR, "src")):
            i : int = 0
            n_split = n.split(os.path.sep)
            while i < src_path_len - 1:
                n_split.pop(0)
                i += 1
            n = ""
            for each in n_split:
                n += (each + (os.path.sep if each != n_split[len(n_split) - 1] else ""))
            for each in f:
                SRCS.append(os.path.join(n, each)) if each.split('.')[-1] == "c" else ""
        
        os.chdir(BUILD_DIR)

        ccflags = []
        if platform.system() == "Windows":
            ccflags = ['-Wall', '-g', '-std=c99', '-D_DEFAULT_SOURCE', '-Wno-missing-braces', MODE, OPT, '-Wl,--subsystem,windows', '-DPLATFORM_DESKTOP']
        elif platform.system() == "Haiku":
            ccflags = ['-Wall', '-std=c99', '-D_DEFAULT_SOURCE', '-Wno-missing-braces', MODE, OPT, '-DPLATFORM_DESKTOP']
        elif platform.system() == "Linux":
            ccflags = ['-Wall', '-std=c99', '-D_DEFAULT_SOURCE', '-Wno-missing-braces', MODE, OPT, '-Wl,-rpath,' + INSTALLPATH, '-DPLATFORM_DESKTOP']
        lflags = []
        if platform.system() == "Windows":
            lflags = [os.path.join(DEPS_DIR, "raylib", "src", "raylib.rc.data"), '-mwindows']
        elif platform.system() == "Haiku":
            pass
        elif platform.system() == "Linux":
            pass
        libs = []
        if platform.system() == "Windows":
            libs = ["raylib", "kuroko", "opengl32", "gdi32", "winmm"]
        elif platform.system() == "Haiku":
            libs = ["raylib", "root", "be", "GL"]
        elif platform.system() == "Linux":
            libs = ["raylib", "kuroko", "GL", "m"]
        _rllibpath = []
        #if platform.system() == "Linux":
        #    _rllibpath = "deps/raylib"
        #else:
        #    _rllibpath = "deps/raylib/src"
        _rllibpath = "deps/raylib/src"

        ctx.program(
            source       = SRCS,
            target       = EXE_NAME,
            includes     = [".", "deps/raylib/src", "deps/raylib/src/external", "deps/raygui/src", "deps/kuroko/src"],
            lib          = libs,
            libpath      = [os.path.join(DEPS_DIR, "kuroko"), os.path.join(DEPS_DIR, "raylib", "src")],
            install_path = INSTALLPATH,
            cflags       = ccflags,
            ldflags      = lflags
        )

        LIB_DIR = ""
        if platform.system() == "Haiku":
            LIB_DIR = "lib"
            #os.mkdir(os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE, LIB_DIR))
            
        ctx.install_files(
            os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE, LIB_DIR),
            ["deps/kuroko/" + krklib_name]
        )

        if platform.system() == "Windows":
            Logs.warn("Copying necessary dlls...")
            f = open("compiler.path", "r")
            compiler_path = f.read().strip()
            f.close()
            shutil.copy(
                os.path.join(compiler_path, "libwinpthread-1.dll"),
                os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE, LIB_DIR)
            )
            Logs.info("Done!")
    
        if DYNAMIC_LINKING:
            ctx.install_files(
                os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE, LIB_DIR),
                [os.path.join(_rllibpath, rllib_name)]
            )

def run(ctx):
    global EXE_NAME
    global DEBUGGER
    os.chdir(os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE))
    call_string = DEBUGGER + " ./" + EXE_NAME
    if platform.system() == "Windows":
        EXE_NAME += ".exe"
        os.system(EXE_NAME)
    elif platform.system() == "Linux":
        os.environ['LD_LIBRARY_PATH'] = INSTALLPATH
        os.system(call_string)
    elif platform.system() == "Haiku":
        os.system(call_string)

def debug(ctx):
    global DEBUGGER
    DEBUGGER = "gdb"
    run(ctx)

def valgrind(ctx):
    global DEBUGGER
    DEBUGGER = "valgrind -s"
    run(ctx)

def valgrind_detailed(ctx):
    global DEBUGGER
    DEBUGGER = "valgrind -s --leak-check=full"
    run(ctx)
