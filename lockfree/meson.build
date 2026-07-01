incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

upp_lockfree_dep = declare_dependency(
    include_directories: incdir,
)

if get_option('tests')
    subdir('test')
endif

