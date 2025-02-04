project(
  'algorithms-bigint',
  'cpp',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
  ],
)

src = ['number.cc', 'mul.cc', 'div.cc']

executable(
  'multiplication',
  src,
  cpp_args: ['-DMUL'],
)

executable(
  'division',
  src,
  cpp_args: ['-DDIV'],
)
