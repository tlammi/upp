TESTS = []
subdir('core')

fs = import('fs')


common_test_deps = [dependency('gtest'), dependency('gtest_main')]
test_deps = {
    'core': [upp_core_dep] + common_test_deps
}

foreach t: TESTS
  path = fs.relative_to(t, '.')
  path = path.split('.')[0]
  parts = path.split('/')
  libname = parts[0]
  nm = '.'.join(parts)
  e = executable(nm, t, dependencies: test_deps[libname])
  test(nm, e)
endforeach
