tests = files('logs.cpp')

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
