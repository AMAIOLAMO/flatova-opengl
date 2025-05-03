.PHONY: compile setup wipe

compile:
	meson compile -C build/

test:
	meson test -C build/

setup:
	meson setup build/

wipe:
	meson setup --wipe build/
