tests = files(
    'intrusive_stack.cpp',
)

fs = import('fs')

deps = [
    upp_lockfree_dep,
    dependency('gtest'),
    dependency('gtest_main'),
]

foreach t : tests
    nm = fs.relative_to(t, meson.current_source_dir())
    nm = fs.replace_suffix(nm, '.test')
    e = executable(nm, t, dependencies: deps)
    test(nm, e, suite: ['unit', 'lockfree'])
endforeach
