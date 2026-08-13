tests = files('logs.cpp', 'manual.cpp')

fs = import('fs')

deps = [
    upp_logs_dep,
]

foreach t : tests
    nm = fs.relative_to(t, meson.current_source_dir())
    nm = fs.replace_suffix(nm, '.test')
    e = executable(nm, t, dependencies: deps)
    test(nm, e, suite: ['unit', 'logs'])
endforeach

unit_tests = files('context.cpp')

deps += [
  dependency('gtest'),
  dependency('gtest_main'),
  dependency('gmock'),
]

foreach t : unit_tests
    nm = fs.relative_to(t, meson.current_source_dir())
    nm = fs.replace_suffix(nm, '.test')
    e = executable(nm, t, dependencies: deps, include_directories: priv_incdir)
    test(nm, e, suite: ['unit', 'logs'])
endforeach

