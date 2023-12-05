# Aardvark adapter Driver Build Definitions

aardvark_driver_files = files(
	'aardvark/base.cpp',
	'aardvark/i2c.cpp',
	'aardvark/spi.cpp',
	'aardvark/gpio.cpp'
)

aardvark_vendor_include = include_directories('vendor', is_system: true)
aardvark_library_base = meson.current_source_dir() + '/vendor'

if build_machine.system() == 'darwin'
	if build_machine.cpu_family() == 'aarch64'
		aardvark_library_loc = aardvark_library_base + '/mac-arm64'
		message('Linking Mac ARM64 Aardvark Library')
	elif build_machine.cpu_family() == 'x86_64'
		aardvark_library_loc = aardvark_library_base + '/mac-x86_64'
		message('Linking Mac x86_64 Aardvark Library')
	else
		error('Unexpected MacOS CPU Architecture: ' + build_machine.cpu_family())
	endif
elif build_machine.system() == 'linux'
	if build_machine.cpu_family() == 'x86_64'
		aardvark_library_loc = aardvark_library_base + '/linux-x86_64'
		message('Linking Linux x86_64 Aardvark Library')
	else
		error('Unexpected Linux CPU Architecture: ' + build_machine.cpu_family())
	endif
else
	error('This repository does not currently support ' + build_machine.system())
endif


aardvark_vendor_native = static_library('aardvark_vendor_native',
	sources: files('vendor/aardvark.c'),
	include_directories: aardvark_vendor_include,
	c_args: [
		'-DAARDVARK_LIBRARY_PATH="' + aardvark_library_loc + '"'
	],
	native: true
)

aardvark_native = static_library('aardvark_native',
	sources: aardvark_driver_files,
	include_directories: aardvark_vendor_include,
	dependencies: [
		framework_include_dep,
		framework_native_include_dep
	],
	native: true,
	build_by_default: meson.is_subproject() == false
)

clangtidy_files += aardvark_driver_files
