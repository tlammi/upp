incdir = include_directories('include')

SRCS = files()

subdir('lib')

deps = [
  upp_core_dep
]

upp_linux_lib = library('upp-linux', SRCS, include_directories: incdir, dependencies: deps)
upp_linux_dep = declare_dependency(include_directories: incdir, link_with: upp_linux_lib, dependencies: deps)

subdir('test')

