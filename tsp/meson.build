project(
  'algorithms-tsp',
  ['cpp'],
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
  ],
)

executable(
  'main',
  files(
    'main.cc',
    'set.cc',
    'graph.cc',
    'naive.cc',
    'bellmanHeldKarp.cc',
  ),
  cpp_args: '-march=native',
)

gtestDep = dependency('gtest_main')

setTests = executable(
  'tests',
  files(
    'setTests.cc',
    'set.cc',
  ),
  dependencies: [gtestDep],
  cpp_args : '-march=native',
)

test('test set', setTests)
