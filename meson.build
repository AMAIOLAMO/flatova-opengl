project('flatova', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

srcs = files(
  'main.c'
)

includes = [
  include_directories('public')
]

subdir('lib')
subdir('private')

subdir('tests')

deps = []

deps += [dependency('glfw3'), dependency('gl')]
deps += dependency('cglm')
deps += dependency('nativefiledialog-extended')


executable('flatova',
           sources: srcs,
           dependencies: deps,
           link_args: '-lm',
           include_directories: includes,
           install : true)

