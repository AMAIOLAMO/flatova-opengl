project('opengl_test', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

srcs = files(
  'main.c'
)

includes = [
  include_directories('public')
]

subdir('lib')

subdir('private')

glfw3_deps = [dependency('glfw3'), dependency('gl')]
cglm_dep = [dependency('cglm')]
nfde_dep = [dependency('nativefiledialog-extended')]

executable('opengl_test',
           sources: srcs,
           dependencies: [glfw3_deps, cglm_dep, nfde_dep],
           link_args: '-lm',
           include_directories: includes,
           install : true)

