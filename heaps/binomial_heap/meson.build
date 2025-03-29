project(
  'algorithms-binomial-heap',
  'cpp',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
  ],
)

gtest_dep = dependency('gtest_main')

tests = executable(
  'tests',
  files('tests.cc'),
  dependencies: [gtest_dep],
)

test('binomial heap tests', tests)
