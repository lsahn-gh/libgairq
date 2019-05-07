test(
  'autoptr-main',
  executable('autoptr-main', 'autoptr-main.c',
             include_directories: root_dir,
             dependencies: gairq_deps,
             c_args: gairq_c_args,
             link_with: gairq_lib),
  env: [
    'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
    'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
  ],
)

test(
  'city-main',
  executable('city-main', 'city-main.c',
             include_directories: root_dir,
             dependencies: gairq_deps,
             c_args: gairq_c_args,
             link_with: gairq_lib),
  env: [
    'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
    'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
  ],
)
