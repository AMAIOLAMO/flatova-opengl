project('flatova', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

core_srcs = files()
impl_srcs = files()
editor_srcs = files()
srcs = files()

includes = [
  include_directories('public')
]

## IMPORTANT, order must be preserved here, as subdir
## will directly utilize the variable names above
subdir('lib')
subdir('private')

all_srcs = core_srcs + impl_srcs + editor_srcs + srcs

cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required: false)

core_deps = [dependency('gl'), dependency('cglm'), math_dep]
editor_deps = [dependency('glfw3'), dependency('nativefiledialog-extended')]

all_deps = [core_deps, editor_deps]

subdir('tests')

# build core dynlib
# shared_library('flatova_core',
#   sources: all_srcs,
#   dependencies: all_deps,
#   include_directories: includes,
# )

# build editor dynlib
shared_library('flatova_editor',
  sources: all_srcs + files('editor_main.c'),
  dependencies: all_deps,
  include_directories: includes,
)

# lightweight hot-reloading
executable('flatova_editor_run',
  sources: all_srcs + files('editor_runner.c'),
  dependencies: all_deps,
  include_directories: includes,
  install : false
)
