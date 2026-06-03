incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

subdir('lib')

deps = [upp_core_dep]

upp_linux_lib = library(
    'upp-linux',
    SRCS,
    include_directories: incdir,
    dependencies: deps,
    install: true,
)
pkg.generate(upp_linux_lib)
upp_linux_dep = declare_dependency(
    include_directories: incdir,
    link_with: upp_linux_lib,
    dependencies: deps,
)

subdir('test')

