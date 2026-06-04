project(
    'upp',
    'cpp',
    default_options: {'cpp_std': 'c++26'},
    meson_version: '>=1.3.0',
)

pkg = import('pkgconfig')

subdir('core')
subdir('coro')
subdir('fs')
subdir('logs')
subdir('codec')
subdir('linux')

upp_dep = declare_dependency(
    dependencies: [
        upp_core_dep,
        upp_fs_dep,
        upp_logs_dep,
        upp_codec_dep,
        upp_linux_dep,
    ],
)
