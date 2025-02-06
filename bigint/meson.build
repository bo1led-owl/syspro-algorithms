project(
  'algorithms-bigint',
  'cpp',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
  ],
)

src = files('number.cc', 'mul.cc', 'div.cc')
gtest_dep = dependency('gtest_main')

mul = executable(
  'multiplication',
  src,
  dependencies: [gtest_dep],
)

div = executable(
  'division',
  src,
  dependencies: [gtest_dep],
)

test('multiplication', mul)
test('division', div)
