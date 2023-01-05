CROSS =--cross-file=cross-arm.txt --cross-file=cross-samd21.txt

all: build/firmware.elf

build/build.ninja:
	meson setup build $(CROSS) -Db_lto=true

.PHONY: build/firmware.elf
build/firmware.elf: build/build.ninja
	ninja -C build -v

build/firmware.dfu: build/firmware.elf
	dx1elf2dfu build/firmware.elf build/firmware.dfu

.PHONY: dfu
dfu: build/firmware.dfu
	dfu-util -d 1209:2003 -D build/firmware.dfu
