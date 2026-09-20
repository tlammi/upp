incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

#subdir('lib')

#upp_core_lib = library(
#    'upp-core',
#    SRCS,
#    include_directories: incdir,
#    install: true,
#)
#pkg.generate(upp_core_lib)
upp_math_dep = declare_dependency(
    include_directories: incdir,
    dependencies: upp_core_dep,
)

if get_option('tests')
    subdir('test')
endif

