tests = files('signal.cpp', 'dlopen.cpp')

fs = import('fs')

shared_library('dlopentest', files('libdlopentest.cpp'), gnu_symbol_visibility: 'hidden')

deps = [
    upp_linux_dep,
    dependency('gtest'),
    dependency('gtest_main'),
]

foreach t : tests
    nm = fs.relative_to(t, meson.current_source_dir())
    nm = fs.replace_suffix(nm, '.test')
    e = executable(nm, t, dependencies: deps)
    test(nm, e, suite: ['smoke', 'linux'], env: ['BUILD_DIR=' + meson.current_build_dir()])
endforeach
