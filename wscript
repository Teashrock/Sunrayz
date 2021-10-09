VERSION    = "master"
APPNAME    = "sunrayz"
REVISION   = ""
BUILD_TYPE = "debug"

top = '.'
out = "build"

import glob
import os
import platform
import shutil
import subprocess
import sys
import json
from waflib.Tools.compiler_c import c_compiler
from waflib import Logs
c_compiler['win32'] = ['gcc']

RAYLIB_REPO    = "https://github.com/raysan5/raylib"
RAYLIB_VERSION = "3.7.0"

KUROKO_REPO    = "https://github.com/kuroko-lang/kuroko"
KUROKO_VERSION = "1.1.2"

BUILD_DIR = os.getcwd()
SRC_DIR   = os.path.join(BUILD_DIR, "src")

global EXE_NAME
EXE_NAME = "main"

if platform.system() == "Windows":
    MAKE_COMMAND = "mingw32-make"
else:
    MAKE_COMMAND = "make"
MAKE_ARGUMENTS = "-j9"

INSTALLPATH = os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE)

def options(opt):
    if platform.system() == "Windows":
        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    opt.load('compiler_c')

def configure(conf):
    def download(what : str, version : str, repo : str):
        try:
            clistfile = open("downloaded.list", "r")
            clist = clistfile.read().split('\n')
            clistfile.close()
        except:
            clist = []
        if not what in clist:
            tar_path_name = os.path.join(DEPS_DIR, ("{}-{}.tar.gz".format(what, version)))
            if not os.path.exists(os.path.join(os.path.dirname(tar_path_name), tar_path_name.split(os.path.sep)[-1].split('.')[0].split('-')[0])):
                if not os.path.exists(tar_path_name):
                    with open(tar_path_name, "wb") as tarar:
                        Logs.warn("Downloading {}-{}.tar.gz, this may take a while...".format(what, version))
                        try:
                            import requests
                            cnt = requests.get(repo + ("/archive/refs/tags/%s.tar.gz" % version), allow_redirects=True).content
                            if cnt == b"404: Not Found":
                                cnt = requests.get(repo + ("/archive/refs/tags/v%s.tar.gz" % version), allow_redirects=True).content
                        except ImportError:
                            import urllib.request
                            cnt = urllib.request.urlopen(repo + ("/archive/refs/tags/%s.tar.gz") % version).read()
                            if cnt == b"404: Not Found":
                                cnt = urllib.request.urlopen(repo + ("/archive/refs/tags/v%s.tar.gz") % version).read()
                        tarar.write(cnt)
                import tarfile
                tf = tarfile.open(tar_path_name, 'r')
                Logs.warn("Extracting...")
                tf.extractall(DEPS_DIR)
                shutil.move(os.path.join(DEPS_DIR, "{}-{}".format(what, version)), os.path.join(DEPS_DIR, what))
                Logs.info("Done!")
            clistfile = open("downloaded.list", "w")
            clistfile.write(what)
            clistfile.close()

    if platform.system() == "Windows":
	    os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    conf.load('compiler_c')
    conf.env.THIS = os.getcwd()
    DEPS_DIR = os.path.join(conf.env.THIS, "deps")
    Logs.warn("Configuring...")
    if not os.path.exists(DEPS_DIR): os.mkdir(DEPS_DIR)
    download("raylib", RAYLIB_VERSION, RAYLIB_REPO)
    download("kuroko", KUROKO_VERSION, KUROKO_REPO)
    
    p = ""
    Logs.warn("Changing Raylib buld type to shared... ")
    with open("deps/raylib/src/Makefile", "r") as rf:
        p = rf.read().replace("RAYLIB_LIBTYPE       ?= STATIC", "RAYLIB_LIBTYPE       ?= SHARED", 1)
    
    with open("deps/raylib/src/Makefile", "w") as rf:
        rf.write(p)
    
    clistfile = open("downloaded.list", "r")
    clist = clistfile.read().split('\n')
    if platform.system() == "Linux" and not "raylib" in clist:
        os.chdir("deps/raylib/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", BUILD_DIR + "/patches/raylib_makefile_linux.patch"])
        sp.wait()
        os.chdir(BUILD_DIR)
    
    if platform.system() == "Haiku" and not "raylib" in clist:
        Logs.warn("Patching Raylib Makefile...")
        os.chdir("deps/raylib/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", BUILD_DIR + "/patches/raylib_makefile_haiku.patch"])
        sp.wait()
        os.chdir(BUILD_DIR)

        Logs.warn("Applying special Raylib patch for Haiku OS...")
        os.chdir("deps/raylib")
        sp = subprocess.Popen(["patch", "-Np1", "-i", BUILD_DIR + "/patches/raylib_haiku.patch"])
        sp.wait()
        os.chdir(BUILD_DIR)
    Logs.info("All done here!")

    if platform.system() == "Windows" and not "kuroko" in clist:
        Logs.warn("Patching Kuroko rline.c... ")
        with open("deps/kuroko/src/vendor/rline.c", "r") as kf:
            p = kf.read().replace('#include "wcwidth._h"', "#include \"wcwidth._h\"\n#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200\n#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004", 1)
        with open("deps/kuroko/src/vendor/rline.c", "w") as kf:
            kf.write(p)
        Logs.info("Done!")
    elif platform.system() == "Haiku" and not "kuroko" in clist:
        Logs.warn("Patching Kuroko Makefile... ")
        #with open("deps/kuroko/Makefile", "r") as kf:
        #p = kf.read().replace('CFLAGS  += -Wno-format -static-libgcc -pthread', "CFLAGS  += -Wno-format -static-libgcc", 1)
        #p = kf.read().replace("\nCFLAGS  += -pthread\nLDLIBS  += -ldl -lpthread\nBIN_FLAGS = -rdynamic", "\n", 1)
        #p = kf.read().replace('${CC} ${CFLAGS} ${LDFLAGS} -fPIC -shared -o $@ ${SOOBJS} ${LDLIBS}', '${CC} ${CFLAGS} ${LDFLAGS} -shared -o $@ ${SOOBJS} ${LDLIBS}', 1)
        #p = kf.read().replace('${CC} ${CFLAGS} ${LDFLAGS} -fPIC -shared -o $@ ${SOOBJS} ${WINLIBS} -Wl,--export-all-symbols,--out-implib,libkuroko.a', '${CC} ${CFLAGS} ${LDFLAGS} -shared -o $@ ${SOOBJS} ${WINLIBS} -Wl,--export-all-symbols,--out-implib,libkuroko.a')
        os.chdir("deps/kuroko")
        sp = subprocess.Popen(["patch", "-Np1", "-i", BUILD_DIR + "/patches/kuroko_haiku.patch"])
        sp.wait()
        Logs.info("Done!")

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
        os.remove(os.path.join(BUILD_DIR, "downloaded.list"))
    except: pass

def distclean(ctx):
    purge(ctx)

def build(ctx):
    os.chdir(BUILD_DIR)
    if platform.system() == "Windows":
        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    DEPS_DIR = os.path.join(os.getcwd(), "deps")
    if ctx.cmd == "clean":
        if platform.system() == "Windows":
            os.environ["CC"] = "i686-w64-mingw32-gcc"
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
                    os.environ["CC"] = "i686-w64-mingw32-gcc"
            # mingw32-make
            if platform.system() == "Haiku":
                os.environ["KRK_DISABLE_THREADS"] = "1"
            sp = subprocess.Popen([MAKE_COMMAND, MAKE_ARGUMENTS])
            sp.wait()
            os.chdir(BUILD_DIR)
            clistfile.write("kuroko\n")

        rllib_name = ""
        if platform.system() == "Windows":
            rllib_name = "raylib.dll"
        else:
            # VER_EXT = ""
            # for i in RAYLIB_VERSION:
            #     if not i == '.':
            #         VER_EXT += i
            # rllib_name = "libraylib.so." + VER_EXT
            rllib_name = "libraylib.so"
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
        for i in glob.glob("*.c"):
            SRCS.append("src/" + i)
        
        os.chdir(BUILD_DIR)

        ccflags = []
        if platform.system() == "Windows":
            ccflags = ['-Wall', '-std=c99', '-D_DEFAULT_SOURCE', '-Wno-missing-braces', MODE, OPT, '-Wl,--subsystem,windows', '-DPLATFORM_DESKTOP']
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
            libs = ["raylib", "kuroko", "GL"]
        _rllibpath = []
        if platform.system() == "Linux":
            _rllibpath = "deps/raylib"
        else:
            _rllibpath = "deps/raylib/src"

        ctx.program(
            source       = SRCS,
            target       = EXE_NAME,
            includes     = [".", "deps/raylib/src", "deps/raylib/src/external", "deps/kuroko/src"],
            lib          = libs,
            libpath      = [os.path.join(DEPS_DIR, "kuroko"), os.path.join(DEPS_DIR, "raylib")],
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
    
        ctx.install_files(
            os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE, LIB_DIR),
            [os.path.join(_rllibpath, rllib_name)]
        )

def run(ctx):
    global EXE_NAME
    os.chdir(os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE))
    if platform.system() == "Windows":
        EXE_NAME += ".exe"
        os.system(EXE_NAME)
    elif platform.system() == "Linux":
        os.environ['LD_LIBRARY_PATH'] = INSTALLPATH
        os.system("./" + EXE_NAME)
    elif platform.system() == "Haiku":
        os.system("./" + EXE_NAME)
