subdir('include')

incdir = include_directories('include')

SRCS = files()
subdir('lib')

upp_net = library('upp-net', SRCS, dependencies: upp_core_dep, include_directories: incdir)

upp_net_dep = declare_dependency(link_with: upp_net, include_directories: incdir)
