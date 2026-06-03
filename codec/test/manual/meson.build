tests = files('hwaccel.cpp', 'void_player.cpp')

fs = import('fs')


deps = [upp_codec_dep, upp_fs_dep]

foreach t : tests
    nm = fs.relative_to(t, meson.current_source_dir())
    nm = fs.replace_suffix(nm, '')
    nm = nm.replace('_', '-')
    executable(nm, t, dependencies: deps)
endforeach
