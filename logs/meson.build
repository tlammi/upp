incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

subdir('lib')

deps = [
  upp_core_dep,
]

upp_logs_lib = library('upp-logs', SRCS, include_directories: incdir, dependencies: deps, install: true)
upp_logs_dep = declare_dependency(include_directories: incdir, link_with: upp_logs_lib, dependencies: deps)

subdir('test')

