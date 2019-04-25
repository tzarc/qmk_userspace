export PATH := $(HOME)/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$(PATH)

all: bin

update:
	git submodule update --remote
	make -C qmk_firmware git-submodule

clean:
	rm *.bin || true
	make -C qmk_firmware clean || true

distclean:
	rm *.bin || true
	make -C qmk_firmware distclean || true

CYCLONE_DEPS = $(shell find qmk_firmware/keyboards/tzarc/cyclone -type f)

bin: tzarc_cyclone_default.bin

tzarc_cyclone_default.bin: $(CYCLONE_DEPS)
	make -j$(shell nproc) -C "qmk_firmware" tzarc/cyclone:default
	cp "qmk_firmware/tzarc_cyclone_default.bin" "."

flash: bin
	openocd -s /usr/share/openocd/scripts -f stlink.cfg -c "program tzarc_cyclone_default.bin 0x08000000 verify" -c "reset" -c "exit"

debug: bin
	arm-none-eabi-gdb --command=./gdbinit --exec=./qmk_firmware/.build/tzarc_cyclone_default.elf --symbols=./qmk_firmware/.build/tzarc_cyclone_default.elf

CYCLONE_FORMATTABLE_FILES = $(shell find qmk_firmware/keyboards/tzarc/cyclone -maxdepth 1 -type f \( -name 'cyclone*.h' -or -name 'cyclone*.c' -or -name 'cyclone*.cpp' \))
format:
	@for file in $(CYCLONE_FORMATTABLE_FILES); do clang-format -i $$file ; done
