
import os
import multiprocessing as mp
env = Environment(CPPPATH=["#include"], CXXFLAGS=[
                  "--std=c++17"])

# This enables color prints from subprocesses
env['ENV']['TERM'] = os.environ['TERM']

env.SetOption("num_jobs", mp.cpu_count())

SConscript("src/SConscript",
           variant_dir="build/src", duplicate=0, exports="env")

SConscript("unit-test/SConscript",
           variant_dir="build/unit-test", duplicate=0, exports="env")
