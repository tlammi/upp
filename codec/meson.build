incdir = include_directories('include')

SRCS = files()

deps = [
  upp_core_dep,
  upp_logs_dep,
  dependency('libavcodec'),
  dependency('libavutil'),
  dependency('libavformat'),
]

subdir('lib')


upp_codec_lib = library('upp-codec', SRCS, include_directories: incdir, dependencies: deps)
upp_codec_dep = declare_dependency(include_directories: incdir, link_with: upp_codec_lib, dependencies: deps)

subdir('test')

