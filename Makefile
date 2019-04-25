export PATH := $(HOME)/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$(PATH)

all: bin

update:
	git submodule update --remote
	make -C qmk_firmware git-submodule

clean:
	rm *.bin
	make -C qmk_firmware clean

distclean:
	rm *.bin
	make -C qmk_firmware distclean

CYCLONE_SOURCE_FILES = $(shell find qmk_firmware/keyboards/tzarc/cyclone -maxdepth 1 -type f \( -name 'cyclone*.h' -or -name 'cyclone*.c' -or -name 'cyclone*.cpp' \))
format:
	@for file in $(CYCLONE_SOURCE_FILES); do clang-format -i $$file ; done

bin: tzarc_cyclone_default.bin

tzarc_cyclone_default.bin:
	@echo "--------------------------------"
	@echo "Building $@"
	make -j16 -C "qmk_firmware" tzarc/cyclone:default
	cp "qmk_firmware/tzarc_cyclone_default.bin" "."

flash: bin
	@echo "--------------------------------"
	@echo "Flashing..."
	openocd -s /usr/share/openocd/scripts -f stlink.cfg -c "program tzarc_cyclone_default.bin 0x08000000 verify" -c "reset" -c "exit"

debug: bin
	@echo "--------------------------------"
	@echo "Debugging..."
	@arm-none-eabi-gdb --command=./gdbinit --exec=./qmk_firmware/.build/tzarc_cyclone_default.elf --symbols=./qmk_firmware/.build/tzarc_cyclone_default.elf
