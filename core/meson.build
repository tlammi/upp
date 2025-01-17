incdir = include_directories('include')

SRCS = files()

subdir('lib')

upp_core_lib = library('upp-core', SRCS, include_directories: incdir)
upp_core_dep = declare_dependency(include_directories: incdir, link_with: upp_core_lib)

subdir('test')

