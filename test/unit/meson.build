TESTS = []
subdir('core')
if get_option('net')
  subdir('net')
endif

if get_option('async')
  subdir('async')
endif

fs = import('fs')


common_test_deps = [dependency('gtest'), dependency('gtest_main')]
test_deps = {
    'core': [upp_core_dep] + common_test_deps,
    'net': [upp_net_dep] + common_test_deps,
    'async': [upp_async_dep] + common_test_deps,
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
