project('flatova', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

srcs = files()

includes = [
  include_directories('public')
]

subdir('lib')
subdir('private')

deps = []

deps += [dependency('glfw3'), dependency('gl')]
deps += dependency('cglm')
deps += dependency('nativefiledialog-extended')

subdir('tests')

executable('flatova',
  sources: srcs + files('main.c'),
  dependencies: deps,
  link_args: '-lm',
  include_directories: includes,
  install : true
)

