incdir = include_directories('include')

SRCS = files()

subdir('lib')

deps = [upp_core_dep]


upp_fs_lib = library('upp-fs', SRCS, include_directories: incdir, dependencies: deps)
upp_fs_dep = declare_dependency(include_directories: incdir, link_with: upp_fs_lib, dependencies: deps)

subdir('test')


