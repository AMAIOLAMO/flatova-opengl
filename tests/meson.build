ecs_tests = executable(
  'ecs_tests',

  sources: srcs + files('ecs_tests.c'),
  include_directories: includes,
  dependencies: deps,

  link_args: '-lm'
)

test('Entity Component System Tests', ecs_tests)

animation_tests = executable(
  'animation_tests',
  sources: srcs + files('animation_tests.c'),
  include_directories: includes,
  dependencies: deps,

  link_args: '-lm'
)
test('Fl Animation Tests', animation_tests)
