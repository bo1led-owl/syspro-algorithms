project(
  'algorithms-matrix-multiplication',
  ['c', 'cpp'],
  default_options: [
    'c_std=c23',
    'cpp_std=c++23',
    'warning_level=3',
  ],
)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m')

executable(
  'main',
  files('main.c', 'matrix.c'),
  c_args : '-march=native',
  dependencies: [m_dep],
)

gtest_dep = dependency('gtest_main')
tests = executable(
  'tests',
  files('tests.cc', 'matrix.c'),
  dependencies: [gtest_dep],
  c_args : '-march=native',
  cpp_args : '-march=native',
)

test('test algorithms', tests)
