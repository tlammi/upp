incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

subdir('lib')

deps = [
  upp_core_dep,
  dependency('stdexec', fallback: ['stdexec', 'stdexec_dep']),
]

upp_coro_lib = library(
    'upp-coro',
    SRCS,
    include_directories: incdir,
    install: true,
    dependencies: deps,
)

pkg.generate(upp_coro_lib)

upp_coro_dep = declare_dependency(
    include_directories: incdir,
    link_with: upp_coro_lib,
    dependencies: deps,
)

if get_option('tests')
    subdir('test')
endif

