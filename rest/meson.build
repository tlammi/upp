incdir = include_directories('include')

SRCS = files()

subdir('lib')

boost_dep = dependency('boost', modules: ['beast'])

deps = [
  upp_core_dep,
  upp_logs_dep,
]

upp_rest_lib = library('upp-rest', SRCS, include_directories: incdir, dependencies: deps)
upp_rest_dep = declare_dependency(include_directories: incdir, link_with: upp_rest_lib, dependencies: deps)

subdir('test')

