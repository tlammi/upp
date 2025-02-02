incdir = include_directories('include')

subdir('include')
SRCS = files()
subdir('lib')

upp_async = library('upp-async', SRCS, dependencies: upp_core_dep, include_directories: incdir)

upp_async_dep = declare_dependency(link_with: upp_async, dependencies: upp_core_dep, include_directories: incdir)
