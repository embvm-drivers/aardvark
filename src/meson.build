# Aardvark adapter Driver Build Definitions

aardvark_driver_files = files(
	'aardvark/base.cpp',
	'aardvark/i2c.cpp',
	'aardvark/spi.cpp',
	'aardvark/gpio.cpp'
)

aardvark_vendor_include = include_directories('vendor', is_system: true)
aardvark_library_loc = meson.current_source_dir() + '/vendor'

aardvark_native = static_library('aardvark_native',
	c_args: [
		'-DAARDVARK_LIBRARY_PATH="' + aardvark_library_loc + '"'
	],
	sources: [
		aardvark_driver_files,
		files('vendor/aardvark.c')
	],
	include_directories: [
		aardvark_vendor_include,
	],
	dependencies: [
		framework_include_dep,
		framework_native_include_dep
	],
	native: true,
	build_by_default: meson.is_subproject() == false
)

clangtidy_files += aardvark_driver_files
