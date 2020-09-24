# Aardvark adapter Driver Build Definitions

aardvark_driver_files = files(
	'base.cpp',
	'i2c.cpp',
	'spi.cpp',
	'vendor/aardvark.c'
	)

aardvark_vendor_include = include_directories('vendor', is_system: true)

aardvark_library_loc = meson.current_source_dir() + '/vendor'

aardvark_driver_dep = declare_dependency(
	sources: aardvark_driver_files,
	include_directories: [driver_include_root, aardvark_vendor_include],
	compile_args: '-DAARDVARK_LIBRARY_PATH="' + aardvark_library_loc + '"'
)

framework_lint_files += files('base.cpp', 'i2c.cpp', 'spi.cpp')
