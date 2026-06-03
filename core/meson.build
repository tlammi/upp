incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

subdir('lib')

upp_core_lib = library(
    'upp-core',
    SRCS,
    include_directories: incdir,
    install: true,
)
pkg.generate(upp_core_lib)
upp_core_dep = declare_dependency(
    include_directories: incdir,
    link_with: upp_core_lib,
)


subdir('test')

