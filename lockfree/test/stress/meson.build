tests = files(
    'intrusive_stack.cpp',
)

fs = import('fs')

deps = [
    upp_lockfree_dep,
    upp_core_dep,
]

foreach t : tests
    nm = fs.relative_to(t, meson.current_source_dir())
    nm = fs.replace_suffix(nm, '.stress')
    e = executable(nm, t, dependencies: deps)
    test(nm, e, suite: ['stress', 'lockfree'])
endforeach
