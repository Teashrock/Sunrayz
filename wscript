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

def options(opt):
    if platform.system() == "Windows":
        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    opt.load('compiler_c')

def configure(conf):
    if platform.system() == "Windows":
	    os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    conf.load('compiler_c')
    conf.env.THIS = os.getcwd()
    DEPS_DIR = os.path.join(conf.env.THIS, "deps")
    Logs.warn("Configuring...")
    if not os.path.exists(DEPS_DIR): os.mkdir(DEPS_DIR)
    tar_path_name = os.path.join(DEPS_DIR, ("raylib-%s.tar.gz" % RAYLIB_VERSION))
    if not os.path.exists(os.path.join(os.path.dirname(tar_path_name), tar_path_name.split(os.path.sep)[-1].split('.')[0].split('-')[0])):
        if not os.path.exists(tar_path_name):
            with open(tar_path_name, "wb") as tarar:
                Logs.warn("Downloading raylib-%s.tar.gz, this may take a while..." % RAYLIB_VERSION)
                try:
                    import requests
                    tarar.write(requests.get(RAYLIB_REPO + ("/archive/refs/tags/%s.tar.gz" % RAYLIB_VERSION), allow_redirects=True).content)
                except ImportError:
                    import urllib.request
                    tarar.write(urllib.request.urlopen(RAYLIB_REPO + ("/archive/refs/tags/%s.tar.gz") % RAYLIB_VERSION).read())
        import tarfile
        tf = tarfile.open(tar_path_name, 'r')
        Logs.warn("Extracting...")
        tf.extractall(DEPS_DIR)
        shutil.move(os.path.join(DEPS_DIR, ("raylib-%s" % RAYLIB_VERSION)), os.path.join(DEPS_DIR, "raylib"))
        Logs.info("Done!")
    
    tar_path_name = os.path.join(DEPS_DIR, ("kuroko-%s.tar.gz" % KUROKO_VERSION))
    if not os.path.exists(os.path.join(os.path.dirname(tar_path_name), tar_path_name.split(os.path.sep)[-1].split('.')[0].split('-')[0])):
        if not os.path.exists(tar_path_name):
            with open(tar_path_name, "wb") as tarar:
                Logs.warn("Downloading kuroko-%s.tar.gz, this may take a while..." % KUROKO_VERSION)
                try:
                    import requests
                    tarar.write(requests.get(KUROKO_REPO + ("/archive/refs/tags/v%s.tar.gz" % KUROKO_VERSION), allow_redirects=True).content)
                except ImportError:
                    import urllib.request
                    tarar.write(urllib.request.urlopen(KUROKO_REPO + ("/archive/refs/tags/v%s.tar.gz") % KUROKO_VERSION).read())
        import tarfile
        tf = tarfile.open(tar_path_name, 'r')
        Logs.warn("Extracting...")
        tf.extractall(DEPS_DIR)
        shutil.move(os.path.join(DEPS_DIR, ("kuroko-%s" % KUROKO_VERSION)), os.path.join(DEPS_DIR, "kuroko"))
        Logs.info("Done!")
    
    p = ""
    Logs.warn("Patching Raylib Makefile... ")
    with open("deps/raylib/src/Makefile", "r") as rf:
        p = rf.read().replace("RAYLIB_LIBTYPE       ?= STATIC", "RAYLIB_LIBTYPE       ?= SHARED", 1)
    
    with open("deps/raylib/src/Makefile", "w") as rf:
        rf.write(p)
    
    if platform.system() == "Haiku":
        os.chdir("deps/raylib/src")
        sp = subprocess.Popen(["patch", "-Np1", "-i", BUILD_DIR + "/patches/raylib_makefile.patch"])
        sp.wait()
        os.chdir(BUILD_DIR)
    
    if platform.system() == "Haiku":
        Logs.warn("Applying special patch for Haiku OS...")
        os.chdir("deps/raylib")
        sp = subprocess.Popen(["patch", "-Np1", "-i", BUILD_DIR + "/patches/raylib.patch"])
        sp.wait()
        os.chdir(BUILD_DIR)
    Logs.info("All done here!")

    if platform.system() == "Windows":
        Logs.warn("Patching Kuroko rline.c... ")
        with open("deps/kuroko/src/vendor/rline.c", "r") as kf:
            p = kf.read().replace('#include "wcwidth._h"', "#include \"wcwidth._h\"\n#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200\n#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004", 1)
        with open("deps/kuroko/src/vendor/rline.c", "w") as kf:
            kf.write(p)
        Logs.info("Done!")
    elif platform.system() == "Haiku":
        Logs.warn("Patching Kuroko Makefile... ")
        #with open("deps/kuroko/Makefile", "r") as kf:
        #p = kf.read().replace('CFLAGS  += -Wno-format -static-libgcc -pthread', "CFLAGS  += -Wno-format -static-libgcc", 1)
        #p = kf.read().replace("\nCFLAGS  += -pthread\nLDLIBS  += -ldl -lpthread\nBIN_FLAGS = -rdynamic", "\n", 1)
        #p = kf.read().replace('${CC} ${CFLAGS} ${LDFLAGS} -fPIC -shared -o $@ ${SOOBJS} ${LDLIBS}', '${CC} ${CFLAGS} ${LDFLAGS} -shared -o $@ ${SOOBJS} ${LDLIBS}', 1)
        #p = kf.read().replace('${CC} ${CFLAGS} ${LDFLAGS} -fPIC -shared -o $@ ${SOOBJS} ${WINLIBS} -Wl,--export-all-symbols,--out-implib,libkuroko.a', '${CC} ${CFLAGS} ${LDFLAGS} -shared -o $@ ${SOOBJS} ${WINLIBS} -Wl,--export-all-symbols,--out-implib,libkuroko.a')
        os.chdir("deps/kuroko")
        sp = subprocess.Popen(["patch", "-Np1", "-i", BUILD_DIR + "/patches/kuroko.patch"])
        sp.wait()
        Logs.info("Done!")

def build(ctx):
    os.chdir(BUILD_DIR)
    if platform.system() == "Windows":
        os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    DEPS_DIR = os.path.join(os.getcwd(), "deps")
    if ctx.cmd == "clean":
        if platform.system() == "Windows":
            os.environ["CC"] = "i686-w64-mingw32-gcc"
            os.chdir(os.path.join(DEPS_DIR, "kuroko"))
            sp = subprocess.Popen(MAKE_COMMAND, "clean")
            sp.wait()
        os.chdir(os.path.join(DEPS_DIR, "raylib", "src"))
        sp = subprocess.Popen(MAKE_COMMAND, "clean")
        sp.wait()
        shutil.rmtree(os.path.join(BUILD_DIR, "result"))
    else:
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
        if not os.path.exists(os.path.join(DEPS_DIR, "kuroko", krklib_name)):
            # Kuroko build
            Logs.warn("Building Kuroko...")
            # cd kuroko
            os.chdir(os.path.join(DEPS_DIR, "kuroko"))
            if platform.system() == "Windows":
                # set CC=i686-w64-mingw32-gcc
                os.environ["CC"] = "i686-w64-mingw32-gcc"
        # mingw32-make
        os.environ["KRK_DISABLE_THREADS"] = "1"
        sp = subprocess.Popen(MAKE_COMMAND)
        sp.wait()
        os.chdir(BUILD_DIR)

        rllib_name = ""
        if platform.system() == "Windows":
            rllib_name = "raylib.dll"
        else:
            VER_EXT = ""
            for i in RAYLIB_VERSION:
                if not i == '.':
                    VER_EXT += i
            rllib_name = "libraylib.so." + VER_EXT
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
            sp = subprocess.Popen(MAKE_COMMAND)
            sp.wait()
            os.chdir(BUILD_DIR)

        os.chdir(SRC_DIR)

        SRCS = []
        for i in glob.glob("*.c"):
            SRCS.append("src/" + i)
        
        os.chdir(BUILD_DIR)

        ccflags = []
        if platform.system() == "Windows":
            ccflags = ['-Wall', '-std=c99', '-D_DEFAULT_SOURCE', '-Wno-missing-braces', MODE, OPT, '-Wl,--subsystem,windows', '-DPLATFORM_DESKTOP']
        else:
            ccflags = ['-Wall', '-std=c99', '-D_DEFAULT_SOURCE', '-Wno-missing-braces', MODE, OPT, '-DPLATFORM_DESKTOP']
        lflags = []
        if platform.system() == "Windows":
            lflags = [os.path.join(DEPS_DIR, "raylib", "src", "raylib.rc.data"), '-mwindows']
        elif platform.system() == "Haiku":
            pass
        libs = []
        if platform.system() == "Windows":
            libs = ["raylib", "kuroko", "opengl32", "gdi32", "winmm"]
        elif platform.system() == "Haiku":
            libs = ["raylib", "root", "be", "GL"]

        ctx.program(
            source       = SRCS,
            target       = EXE_NAME,
            includes     = [".", "deps/raylib/src", "deps/raylib/src/external", "deps/kuroko/src"],
            lib          = libs,
            libpath      = [os.path.join(DEPS_DIR, "kuroko"), os.path.join(DEPS_DIR, "raylib", "src")],
            install_path = os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE),
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
            ["deps/raylib/src/" + rllib_name]
        )

def run(ctx):
    global EXE_NAME
    os.chdir(os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE))
    if platform.system() == "Windows":
        EXE_NAME += ".exe"
    os.system(EXE_NAME)
