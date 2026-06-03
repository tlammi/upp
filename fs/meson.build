incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

subdir('lib')

deps = [upp_core_dep]


upp_fs_lib = library(
    'upp-fs',
    SRCS,
    include_directories: incdir,
    dependencies: deps,
    install: true,
)
pkg.generate(upp_fs_lib)
upp_fs_dep = declare_dependency(
    include_directories: incdir,
    link_with: upp_fs_lib,
    dependencies: deps,
)

if get_option('tests')
    subdir('test')
endif

