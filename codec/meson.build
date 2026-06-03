incdir = include_directories('include')
install_subdir('include', install_dir: '.')

SRCS = files()

deps = [
    upp_core_dep,
    upp_logs_dep,
    dependency('libavcodec'),
    dependency('libavutil'),
    dependency('libavformat'),
]

subdir('lib')


upp_codec_lib = library(
    'upp-codec',
    SRCS,
    include_directories: incdir,
    dependencies: deps,
    install: true,
)
upp_codec_dep = declare_dependency(
    include_directories: incdir,
    link_with: upp_codec_lib,
    dependencies: deps,
)

subdir('test')

