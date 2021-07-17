VERSION    = "0.0.1"
APPNAME    = "raybook"
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
KUROKO_VERSION = "1.1.1"

BUILD_DIR = os.getcwd()
SRC_DIR   = os.path.join(BUILD_DIR, "src")

EXE_NAME = "main"

def options(opt):
    os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    #conf.env.CC = "i686-w64-mingw32-gcc"
    opt.load('compiler_c')

def configure(conf):
    os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    #conf.env.CC = "i686-w64-mingw32-gcc"
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
    
    p : str
    Logs.warn("Patching Raylib Makefile... ")
    with open("deps/raylib/src/Makefile", "r") as rf:
        p = rf.read().replace("RAYLIB_LIBTYPE       ?= STATIC", "RAYLIB_LIBTYPE       ?= SHARED", 1)
    with open("deps/raylib/src/Makefile", "w") as rf:
        rf.write(p)
    Logs.info("Done!")

    Logs.warn("Patching Kuroko rline.c... ")
    with open("deps/kuroko/src/vendor/rline.c", "r") as kf:
        p = kf.read().replace('#include "wcwidth._h"', "#include \"wcwidth._h\"\n#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200\n#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004", 1)
    with open("deps/kuroko/src/vendor/rline.c", "w") as kf:
        kf.write(p)
    Logs.info("Done!")

def build(ctx):
    os.environ["PATH"] = "C:\\mingw32\\bin;C:\\mingw32\\libexec\\gcc\\i686-w64-mingw32\\8.1.0;" + os.environ["PATH"]
    DEPS_DIR = os.path.join(os.getcwd(), "deps")
    if ctx.cmd == "clean":
        os.environ["CC"] = "i686-w64-mingw32-gcc"
        os.chdir(os.path.join(DEPS_DIR, "kuroko"))
        sp = subprocess.Popen("mingw32-make", "clean")
        sp.wait()
        os.chdir(os.path.join(DEPS_DIR, "raylib", "src"))
        sp = subprocess.Popen("mingw32-make", "clean")
        sp.wait()
        shutil.rmtree(os.path.join(BUILD_DIR, "result"))
    else:
        MODE : str
        OPT : str
        if BUILD_TYPE  == "debug":
            MODE = "-g"
            OPT  = "-O0"
        else:
            MODE = "-s"
            OPT  = "-O1"
        if not os.path.exists(os.path.join(os.getcwd(), "result", platform.system() + "-" + BUILD_TYPE)):
            os.makedirs(os.path.join(os.getcwd(), "result", platform.system() + "-" + BUILD_TYPE))
        if not os.path.exists(os.path.join(DEPS_DIR, "kuroko", "libkuroko.dll")):
            # Kuroko build
            Logs.warn("Building Kuroko...")
            # cd kuroko
            os.chdir(os.path.join(DEPS_DIR, "kuroko"))
            # set CC=i686-w64-mingw32-gcc
            os.environ["CC"] = "i686-w64-mingw32-gcc"
            # mingw32-make
            sp = subprocess.Popen("mingw32-make")
            sp.wait()
            os.chdir(BUILD_DIR)

        if not os.path.exists(os.path.join(DEPS_DIR, "raylib", "src", "raylib.dll")):
            # Raylib build
            Logs.warn("\nBuilding Raylib...")
            # cd raylib/src
            os.chdir(os.path.join(DEPS_DIR, "raylib", "src"))
            # del /q /s raylib.rc.data raylib.dll.rc.data
            os.remove("raylib.rc.data")
            os.remove("raylib.dll.rc.data")
            # windres raylib.rc -o raylib.rc.data
            # windres raylib.dll.rc -o raylib.dll.rc.data
            os.system("windres raylib.rc -o raylib.rc.data")
            os.system("windres raylib.dll.rc -o raylib.dll.rc.data")
            # mingw32-make
            sp = subprocess.Popen("mingw32-make")
            sp.wait()
            os.chdir(BUILD_DIR)

        os.chdir(SRC_DIR)

        SRCS : list = []
        for i in glob.glob("*.c"):
            SRCS.append("src/" + i)
        
        os.chdir(BUILD_DIR)

        ctx.program(
            source       = SRCS,
            target       = EXE_NAME,
            includes     = [".", "deps/raylib/src", "deps/raylib/src/external", "deps/kuroko/src"],
            lib          = ["raylib", "kuroko", "opengl32", "gdi32", "winmm"],
            libpath      = [os.path.join(DEPS_DIR, "kuroko"), os.path.join(DEPS_DIR, "raylib", "src")],
            install_path = os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE),
            cflags       = ['-Wall', '-std=c99', '-D_DEFAULT_SOURCE', '-Wno-missing-braces', MODE, OPT, '-Wl,--subsystem,windows', '-DPLATFORM_DESKTOP'],
            ldflags      = [os.path.join(DEPS_DIR, "raylib", "src", "raylib.rc.data"), '-mwindows']
        )

        ctx.install_files(
            os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE),
            ["deps/kuroko/libkuroko.dll"]
        )
    
        ctx.install_files(
            os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE),
            ["deps/raylib/src/raylib.dll"]
        )

def run(ctx):
    os.chdir(os.path.join(BUILD_DIR, "result", platform.system() + "-" + BUILD_TYPE))
    os.system(EXE_NAME + ".exe")