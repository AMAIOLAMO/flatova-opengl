.PHONY: compile setup

compile:
	meson compile -C build/

setup:
	meson setup build/
