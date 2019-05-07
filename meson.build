project('gairq', 'c',
  version: '0.1.0',
  default_options: [
    'buildtype=debug'
  ],
  meson_version: '>= 0.50.0',
)

gnome = import('gnome')

version_split = meson.project_version().split('.')
MAJOR_VERSION = version_split[0]
MINOR_VERSION = version_split[1]
MICRO_VERSION = version_split[2]

api_version = '@0@.@1@'.format(MAJOR_VERSION, MINOR_VERSION)

root_dir = include_directories('.')

gairq_c_args = [
  '-DGAIRQ_COMPILATION',
  '-DG_LOG_DOMAIN="Gairq"',
]

subdir('gairq')

if get_option('enable_test')
  subdir('tests')
endif
