project(
  'algorithms-sorts',
  'c',
  default_options: [
    'c_std=c11',
    'warning_level=3',
  ],
)

str_radix_sort = executable(
  'str_radix_sort',
  'string_radix_sort.c',
)

test('string radix sort', str_radix_sort)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m')

executable(
  'partition_comparison',
  files('partition_comparison.c'),
  c_args: '-march=native',
  dependencies: [m_dep],
)