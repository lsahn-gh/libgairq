gairq_sources = [
  'gairq-air-object.c',
  'gairq-city.c',
  'gairq-request.c',
]

gairq_headers = [
  'gairq.h',
  'gairq-air-object.h',
  'gairq-city.h',
  'gairq-debug.h',
  'gairq-request.h',
]

# gairq-version.h
version_conf = configuration_data()
version_conf.set('VERSION', meson.project_version())
version_conf.set('MAJOR_VERSION', MAJOR_VERSION)
version_conf.set('MINOR_VERSION', MINOR_VERSION)
version_conf.set('MICRO_VERSION', MICRO_VERSION)
configure_file(
  input: 'gairq-version.h.in',
  output: 'gairq-version.h',
  configuration: version_conf,
  install: true,
  install_dir: join_paths(get_option('includedir'), 'gairq')
)

# gairq-config.h
config_h = configuration_data()
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
configure_file(
  output: 'gairq-config.h',
  configuration: config_h,
)

gairq_deps = [
  dependency('glib-2.0', version: '>= 2.50'),
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('json-glib-1.0', version: '>= 1.4.0'),
  dependency('rest-0.7', version: '>= 0.7.93'),
]

gairq_lib = shared_library('gairq-' + api_version,
  gairq_sources,
  include_directories: root_dir,
  dependencies: gairq_deps,
  c_args: gairq_c_args,
  install: true,
)

install_headers(gairq_headers, subdir: 'gairq')

pkg = import('pkgconfig')
pkg.generate(
  description: 'A GObject based Air Quality library',
    libraries: gairq_lib,
         name: 'Gairq',
     filebase: 'gairq-' + api_version,
      version: meson.project_version(),
      subdirs: 'gairq',
     requires: 'glib-2.0',
  install_dir: join_paths(get_option('libdir'), 'pkgconfig')
)
