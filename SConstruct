
import os
import multiprocessing as mp

AddOption("--build-debug", dest="debug",
          action="store_true", help="Debug build")

AddOption("--build-cov", dest="cov", action="store_true",
          help="Produce coverage information")

Help("""
Options:
    --build-debug   Build with debug instead of release options
    --build-cov     Build with coverage support

Targets:
""")

cpppath = ["#include"]
cxxflags = ["--std=c++17"]
libs = []

if GetOption("debug"):
    cxxflags += ["-O0", "-g"]
else:
    cxxflags += ["-Os", "-Wall", "-Wextra"]

if GetOption("cov"):
    cxxflags += ["-fprofile-arcs", "-ftest-coverage"]
    libs += ["gcov"]

env = Environment(CPPPATH=cpppath, CXXFLAGS=cxxflags, LIBS=libs)

# This enables color prints from subprocesses
env['ENV']['TERM'] = os.environ['TERM']

env.SetOption("num_jobs", mp.cpu_count())

SConscript("src/SConscript",
           variant_dir="build/src", duplicate=0, exports="env")

SConscript("unit-test/SConscript",
           variant_dir="build/unit-test", duplicate=0, exports="env")

SConscript("samples/SConscript",
           variant_dir="build/samples", duplicate=0, exports="env")
