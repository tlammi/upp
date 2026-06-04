tests = files(
    'bitmask.cpp',
    'guard.cpp',
    'cleanup.cpp',
    'flexible.cpp',
    'ring_buffer.cpp',
    'visit.cpp',
    'strlit.cpp',
    'enum_array.cpp',
    'maybe.cpp',
    'bitset.cpp',
    'enum_set.cpp',
    'intrusive_list.cpp',
)

# Work in progress
#tests += files('queue.cpp')


fs = import('fs')

deps = [
    upp_core_dep,
    dependency('gtest'),
    dependency('gtest_main'),
]

foreach t : tests
    nm = fs.relative_to(t, meson.current_source_dir())
    nm = fs.replace_suffix(nm, '.test')
    e = executable(nm, t, dependencies: deps)
    test(nm, e, suite: ['unit', 'core'])
endforeach
