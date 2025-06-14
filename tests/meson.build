ecs_tests = executable(
  'ecs_tests',

  sources: all_srcs + files('ecs_tests.c'),
  include_directories: includes,
  dependencies: all_deps,

  link_args: '-lm'
)
test('Entity Component System Tests', ecs_tests)

animation_tests = executable(
  'animation_tests',

  sources: all_srcs + files('animation_tests.c'),
  include_directories: includes,
  dependencies: all_deps,

  link_args: '-lm'
)
test('Fl Animation Tests', animation_tests)


core_common_tests = executable(
  'core_common_tests',

  sources: all_srcs + files('core_common_tests.c'),
  include_directories: includes,
  dependencies: all_deps,

  link_args: '-lm'
)
test('Engine Core common functions tests', core_common_tests)
