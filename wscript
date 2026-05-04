VERSION    = "master"
APPNAME    = "sunrayz"
REVISION   = ""
BUILD_TYPE = "debug"
DYNAMIC_LINKING = False

top = '.'
out = "build"

import glob
import os
import platform
import shutil
import subprocess
import sys
import json
from build_utils.asset2code import asset_transform
from waflib.Tools.compiler_c import c_compiler
from waflib import Logs
c_compiler['win32'] = ['gcc']

RAYLIB_REPO    = "https://github.com/raysan5/raylib"
RAYLIB_VERSION = "6.0"

RAYGUI_REPO    = "https://github.com/raysan5/raygui"
RAYGUI_VERSION = "4.0"

LUAJIT_REPO    = "https://github.com/LuaJIT/LuaJIT"
LUAJIT_VERSION = "master"

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

INCS = [".", "deps/raylib/src", "deps/raylib/src/external", "deps/raygui/src", "deps/LuaJIT/src"]

global patch_list
patch_list : list = list()

global download_only
download_only : bool = False

def options(opt):
    #if platform.system() == "Windows":
    #    os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\x86_64-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    opt.load('compiler_c')

def configure(conf):
    delim : str = ""
    ext : str = ""
    if not download_only:
        clangd_conf = open(".clangd", "w")
        clangd_conf.write("CompileFlags:\n  Add:\n")
        if platform.system() == "Windows":
            delim = ";"
            ext = ".exe"
            clangd_conf.write("    - --target=x86_64-w64-windows-gnu\n    - -xc\n    - -std=c99\n")
            for i in INCS:
                if i == ".":
                    clangd_conf.write("    - -I" + BUILD_DIR + "\n")
                else:
                    clangd_conf.write(f"    - -I{BUILD_DIR + "\\" + i.replace("/", "\\")}\n")
            gcc_loc = os.path.dirname(os.path.dirname(shutil.which("gcc"))) # pyright: ignore[reportArgumentType, reportCallIssue, reportOptionalMemberAccess]
            #clangd_conf.write(f" -I, {gcc_loc}\\include, ")
            #clangd_conf.write(f" -I, {gcc_loc}\\x86_64-w64-mingw32\\include ]\n")
        else:
            delim = ":"
        clangd_conf.close()
        util_check_paths : list = os.getenv("PATH").split(delim) # pyright: ignore[reportOptionalMemberAccess]
        for key in UTIL_LIST.keys():
            found : bool = False
            if UTIL_LIST[key] == 0 or \
                (UTIL_LIST[key] == 1 and platform.system() == "Windows") or \
                (UTIL_LIST[key] == 2 and (RAYLIB_VERSION == "master" or \
                    RAYGUI_VERSION == "master")):
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
    def download(what : str, version : str, repo : str, extension: str = "tar.gz") -> None:
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
                shutil.move(os.path.join(CACHE_DIR, what), DEPS_DIR)
            else:
                if not os.path.exists(CACHE_DIR):
                    os.mkdir(CACHE_DIR)
                tar_path_name = os.path.join(CACHE_DIR, ("{}-{}.{}".format(what, version, extension)))
                if not os.path.exists(os.path.join(os.path.dirname(tar_path_name), tar_path_name.split(os.path.sep)[-1].split('.')[0].split('-')[0])):
                    if not os.path.exists(tar_path_name):
                        with open(tar_path_name, "wb") as tarar:
                            if repo.split("://")[1].split('/')[0] == "github.com":
                                Logs.warn("Downloading {}-{}.{}, this may take a while...".format(what, version, extension))
                                try:
                                    import requests
                                    cnt = requests.get(repo + ("/archive/refs/tags/{}.{}".format(version, extension)), allow_redirects=True).content
                                    if cnt == b"404: Not Found":
                                        cnt = requests.get(repo + ("/archive/refs/tags/v{}.{}".format(version, extension)), allow_redirects=True).content
                                except ModuleNotFoundError:
                                    import urllib.request
                                    try:
                                        cnt = urllib.request.urlopen(repo + ("/archive/refs/tags/{}.{}").format(version, extension)).read()
                                    except urllib.error.HTTPError:
                                        cnt = urllib.request.urlopen(repo + ("/archive/refs/tags/v{}.{}").format(version, extension)).read()
                            elif repo.split("://")[1].split('/')[0] == "ftp.gnu.org" and repo == GETTEXT_REPO:
                                Logs.warn("Downloading {}-{}.{}, this may take a while...".format(what, version, extension))
                                try:
                                    import requests
                                    cnt = requests.get(repo + ("/pub/gnu/{}/{}-{}.{}").format(what, what, version, extension), allow_redirects=True).content
                                    if cnt == b"404: Not Found":
                                        pass
                                except ModuleNotFoundError:
                                    import urllib.request
                                    try:
                                        cnt = urllib.request.urlopen(repo + ("/pub/gnu/{}/{}-{}.{}").format(what, what, version, extension)).read()
                                    except urllib.error.HTTPError:
                                        pass
                            tarar.write(cnt)
                    if not os.path.exists(os.path.join(DEPS_DIR, what)):
                        if extension == "tar.gz":
                            import tarfile
                            tf = tarfile.open(tar_path_name, 'r')
                            Logs.warn(("Extracting {}-{}.{}...").format(what, version, extension))
                            tf.extractall(DEPS_DIR)
                        elif extension == "zip":
                            from zipfile import ZipFile
                            Logs.warn(("Extracting {}-{}.{}...").format(what, version, extension))
                            with ZipFile(tar_path_name, 'r') as zip_file:
                                zip_file.extractall(DEPS_DIR)
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
    download("LuaJIT", LUAJIT_VERSION, LUAJIT_REPO, "zip")
    #download("gettext", GETTEXT_VERSION, GETTEXT_REPO)

    if download_only:
        exit()
    
    # Patch zone
    for _, _, f in os.walk(os.path.join(BUILD_DIR, "patches")):
        for i in f:
            patch_list.append(i)
    if platform.system() == "Windows":
        Logs.warn("Converting patches' ends-of-line to CR LF, necessary on Windows...")
        for i in patch_list:
            sp = subprocess.Popen(["unix2dos", os.path.join(BUILD_DIR, "patches", i)])
            sp.wait()
        Logs.info("Done!")

    # Raylib and Raygui patches
    Logs.warn("Applying architecture patches for Raylib and Raygui...")
    os.chdir(BUILD_DIR + "/deps/raylib/src")
    sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, "patches", "raylib_rcore.patch")])
    sp.wait()
    sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, "patches", "raylib_raylib.patch")])
    sp.wait()
    os.chdir(BUILD_DIR + "/deps/raygui/src")
    
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
    
    Logs.warn("Changing Raylib platform backend to PLATFORM_DESKTOP_RGFW...")
    with open("deps/raylib/src/Makefile", "r") as rf:
        p = rf.read().replace("TARGET_PLATFORM   = PLATFORM_DESKTOP_GLFW", "TARGET_PLATFORM   = PLATFORM_DESKTOP_RGFW", 1)
    p = ""
    if DYNAMIC_LINKING:
        Logs.warn("Changing Raylib build type to shared... ")
        with open("deps/raylib/src/Makefile", "r") as rf:
            p = rf.read().replace("RAYLIB_LIBTYPE       ?= STATIC", "RAYLIB_LIBTYPE       ?= SHARED", 1)
    
        with open("deps/raylib/src/Makefile", "w") as rf:
            rf.write(p)
    if BUILD_TYPE == "debug":
        with open("deps/raylib/src/Makefile", "r") as rf:
            p = rf.read().replace("RAYLIB_BUILD_MODE    ?= RELEASE", "RAYLIB_BUILD_MODE    ?= DEBUG", 1)
    
        with open("deps/raylib/src/Makefile", "w") as rf:
            rf.write(p)
    
    if platform.system() == "Windows":
        Logs.warn("Patching LuaJIT Makefile...")
        with open("deps/LuaJIT/Makefile", "r") as rf:
            p = rf.read().replace("""ifeq (,$(findstring Windows,$(OS)))
  HOST_SYS:= $(shell uname -s)
else
  HOST_SYS= Windows
endif""", "HOST_SYS= Windows", 1)
    
        with open("deps/LuaJIT/Makefile", "w") as rf:
            rf.write(p)
            
        with open("deps/LuaJIT/Makefile", "r") as rf:
            p = rf.read().replace("""

ifeq (Darwin,$(TARGET_SYS))
  INSTALL_SONAME= $(INSTALL_DYLIBNAME)
  INSTALL_SOSHORT1= $(INSTALL_DYLIBSHORT1)
  INSTALL_SOSHORT2= $(INSTALL_DYLIBSHORT2)
  LDCONFIG= :
endif""", "", 1)
    
        with open("deps/LuaJIT/Makefile", "w") as rf:
            rf.write(p)

        with open("deps/LuaJIT/src/Makefile", "r") as rf:
            p = rf.read().replace("""ifeq (Windows,$(findstring Windows,$(OS))$(MSYSTEM)$(TERM))
  HOST_SYS= Windows
else
  HOST_SYS:= $(shell uname -s)
  ifneq (,$(findstring MINGW,$(HOST_SYS)))
    HOST_SYS= Windows
    HOST_MSYS= mingw
  endif
  ifneq (,$(findstring MSYS,$(HOST_SYS)))
    HOST_SYS= Windows
    HOST_MSYS= mingw
  endif
  ifneq (,$(findstring CYGWIN,$(HOST_SYS)))
    HOST_SYS= Windows
    HOST_MSYS= cygwin
  endif
endif""", "HOST_SYS= Windows", 1)
    
        with open("deps/LuaJIT/src/Makefile", "w") as rf:
            rf.write(p)

        with open("deps/LuaJIT/src/Makefile", "r") as rf:
            p = rf.read().replace(
                r"GIT_RELVER= [ -d ../.git ] && $(GIT) show -s --format=%ct >luajit_relver.txt 2>/dev/null || cat ../.relver >luajit_relver.txt 2>/dev/null || :",
                r"GIT_RELVER= if exist ..\\.git ( $(GIT) show -s --format=%%ct >luajit_relver.txt ) else ( type ..\\.relver >luajit_relver.txt )",
                1
            )
    
        with open("deps/LuaJIT/src/Makefile", "w") as rf:
            rf.write(p)
    
    if platform.system() == "Linux":
        Logs.warn("Patching Raylib Makefile...")
        os.chdir("deps/raylib/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, "patches", "raylib_makefile_linux.patch")])
        sp.wait()
        os.chdir(BUILD_DIR)
    
    if platform.system() == "Haiku":
        Logs.warn("Patching Raylib Makefile...")
        os.chdir("deps/raylib/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, "patches", "raylib_makefile_haiku.patch")])
        sp.wait()
        os.chdir(BUILD_DIR)

        Logs.warn("Applying special Raylib patch for Haiku OS...")
        os.chdir("deps/raylib")
        sp = subprocess.Popen(["patch", "-Np1", "-i", os.path.join(BUILD_DIR, "patches", "raylib_haiku.patch")])
        sp.wait()
        os.chdir(BUILD_DIR)
    
    Logs.info("All patches were applied successfully!")
    
    Logs.warn("Now beginning to transform fonts...")
    for _, _, f in os.walk(os.path.join("assets", "fonts")):
        for each in f:
            Logs.warn("Transforming " + each + "...")
            if not os.path.exists(BUILD_DIR + "/src/assets_gen/fonts"):
                os.mkdir(BUILD_DIR + "/src/assets_gen/fonts")
            asset_transform(each, "font")
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
    except:
        if platform.system() == "Windows":
            try:
                subprocess.run(["rmdir", "/s", "/q", os.path.join(BUILD_DIR, "deps")], shell=True)
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
    try:
        os.remove(os.path.join(BUILD_DIR, ".clangd"))
    except: pass
    for _, _, f in os.walk(os.path.join(BUILD_DIR, "src", "codegen", "assets", "fonts")):
        for each in f:
            if each.endswith('.c'):
                os.remove(os.path.join(BUILD_DIR, "src", "codegen", "assets", "fonts", each))
    try:
        os.remove(os.path.join(BUILD_DIR, "src", "external", "raygui.c"))
    except: pass
    
    if not os.path.exists(os.path.join("src", "codegen", "assets", "fonts")):
        os.mkdir(os.path.join("src", "codegen", "assets", "fonts"))

    with open(os.path.join("src", "codegen", "assets", "fonts", "fonts.h"), "w") as fonts_h:
        fonts_h.write("#pragma once\n\n// This file only links static fonts with the engine.\n// Edit it only if you know what you're doing.\n")

def distclean(ctx):
    purge(ctx)

def uncache(ctx):
    shutil.rmtree(os.path.join(BUILD_DIR, ".cache"))
    shutil.rmtree(os.path.join(BUILD_DIR, "cache"))

def build(ctx):
    os.chdir(BUILD_DIR)
    if platform.system() == "Windows":
        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\x86_64-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    DEPS_DIR = os.path.join(os.getcwd(), "deps")
    if ctx.cmd == "clean":
        if platform.system() == "Windows":
            os.environ["CC"] = "x86_64-w64-mingw32-gcc"
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
                subprocess.run([MAKE_COMMAND, MAKE_ARGUMENTS])
                os.chdir(BUILD_DIR)
                clistfile.write("raylib\n")

        lualib_name = ""
        if platform.system() == "Windows":
            lualib_name = "lua51.dll"
        else:
            lualib_name = "liblua.so.5.1"
        if not "luajit" in clist:
            Logs.warn("\nBuilding LuaJIT...")
            os.chdir(os.path.join(DEPS_DIR, "LuaJIT"))
            sp = subprocess.Popen([MAKE_COMMAND, "-j1"])
            sp.wait()
            os.chdir(BUILD_DIR)
            clistfile.write("luajit\n")

        clistfile.close()
        os.chdir(SRC_DIR)

        SRCS = []
        src_path_len = len(SRC_DIR.split(os.path.sep))
        for n, _, f in os.walk(SRC_DIR):
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
            libs = ["raylib", "lua51", "opengl32", "gdi32", "winmm"]
        elif platform.system() == "Haiku":
            libs = ["raylib", "lua51", "root", "be", "GL"]
        elif platform.system() == "Linux":
            libs = ["raylib", "lua51", "GL", "m"]
        _rllibpath = []
        #if platform.system() == "Linux":
        #    _rllibpath = "deps/raylib"
        #else:
        #    _rllibpath = "deps/raylib/src"
        _rllibpath = "deps/raylib/src"
        _lualibpath = "deps/LuaJIT/src"

        ctx.program(
            source       = SRCS,
            target       = EXE_NAME,
            includes     = INCS,
            lib          = libs,
            libpath      = [os.path.join(DEPS_DIR, "raylib", "src"), os.path.join(DEPS_DIR, "LuaJIT", "src")],
            install_path = INSTALLPATH,
            cflags       = ccflags,
            ldflags      = lflags
        )

        LIB_DIR = ""
        if platform.system() == "Haiku":
            LIB_DIR = "lib"
            #os.mkdir(os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE, LIB_DIR))

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
        ctx.install_files(
            os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE, LIB_DIR),
            [os.path.join(_lualibpath, lualib_name)]
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
