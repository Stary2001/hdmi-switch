project('hdmi-switch', 'c', default_options: ['optimization=s', 'build.optimization=s'])

subdir('linker-script-as-property')

sam_project = subproject('libsam', default_options: ['samd_variant=samd21b', 'samd_device=SAMD21E16BU'])
sam_library = sam_project.get_variable('sam')

# link_whole is important so newlib stubs are pulled from the library
executable('firmware.elf',
	'main.c',
	'usb_descriptors.c',
	c_args: sam_project.get_variable('samd_defines'),
	include_directories: [ sam_project.get_variable('our_headers'), sam_project.get_variable('tinyusb_headers') ],
	link_whole: sam_library,
	link_args: [
		linker_script_flags
	],
	link_depends: linker_script_deps
)
