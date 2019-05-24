export ROOTDIR := $(shell pwd)
export PATH := $(HOME)/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$(PATH)

all: bin

update:
	git submodule update --remote
	make -C "$(ROOTDIR)/qmk_firmware" git-submodule

clean:
	rm *.bin || true
	make -C "$(ROOTDIR)/qmk_firmware" clean || true

distclean:
	rm *.bin || true
	make -C "$(ROOTDIR)/qmk_firmware" distclean || true

bin: bin_L082 bin_L412

remove_L082:
	@rm -f "$(ROOTDIR)/tzarc_cyclone_L082_default.bin" || true

CYCLONE_L082_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/tzarc/cyclone/L082" -type f)
tzarc_cyclone_L082_default.bin: remove_L082 $(CYCLONE_L082_DEPS)
	make -C "$(ROOTDIR)/qmk_firmware" tzarc/cyclone/L082:default
	cp "$(ROOTDIR)/qmk_firmware/tzarc_cyclone_L082_default.bin" "$(ROOTDIR)"

bin_L082: tzarc_cyclone_L082_default.bin

remove_L412:
	@rm -f "$(ROOTDIR)/tzarc_cyclone_L412_default.bin" || true

CYCLONE_L412_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/tzarc/cyclone/L412" -type f)
tzarc_cyclone_L412_default.bin: remove_L412 $(CYCLONE_L412_DEPS)
	make -C "$(ROOTDIR)/qmk_firmware" tzarc/cyclone/L412:default
	cp "$(ROOTDIR)/qmk_firmware/tzarc_cyclone_L412_default.bin" "$(ROOTDIR)"

bin_L412: tzarc_cyclone_L412_default.bin

flash_L082: stlink tzarc_cyclone_L082_default.bin
	st-flash erase
	st-flash write "$(ROOTDIR)/tzarc_cyclone_L082_default.bin" 0x08000000

flash_L412: stlink tzarc_cyclone_L412_default.bin
	st-flash erase
	st-flash write "$(ROOTDIR)/tzarc_cyclone_L412_default.bin" 0x08000000

debug_L082: bin
	{ cd "$(ROOTDIR)/qmk_firmware" && arm-none-eabi-gdb --command="$(ROOTDIR)/gdbinit" --exec="$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L082_default.elf" --symbols="$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L082_default.elf" ; }

debug_L412: bin
	{ cd "$(ROOTDIR)/qmk_firmware" && arm-none-eabi-gdb --command="$(ROOTDIR)/gdbinit" --exec="$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L412_default.elf" --symbols="$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L412_default.elf" ; }

dump_L082: bin
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L082_default.elf"

dump_L412: bin
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L412_default.elf"

CYCLONE_COMMON_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/tzarc/cyclone" -maxdepth 1 -type f \( -name '*.h' -or -name '*.c' -or -name '*.cpp' \) | sort)
format_common:
	@for file in $(CYCLONE_COMMON_FORMATTABLE_FILES); do echo "clang-format $${file}..." ; clang-format -i "$$file" ; dos2unix "$$file" ; done

CYCLONE_L082_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/tzarc/cyclone/L082" -maxdepth 1 -type f \( -name 'config.h' -or -name 'L082.*' -or -name 'cyclone*' \) | sort)
format_L082:
	@for file in $(CYCLONE_L082_FORMATTABLE_FILES); do echo "clang-format $${file}..." ; clang-format -i "$$file" ; dos2unix "$$file" ; done

CYCLONE_L412_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/tzarc/cyclone/L412" -maxdepth 1 -type f \( -name 'config.h' -or -name 'L412.*' -or -name 'cyclone*' \) | sort)
format_L412:
	@for file in $(CYCLONE_L412_FORMATTABLE_FILES); do echo "clang-format $${file}..." ; clang-format -i "$$file" ; dos2unix "$$file" ; done

format: format_common format_L082 format_L412

stlink: /usr/local/bin/st-flash

/usr/local/bin/st-flash:
	sudo apt install -y build-essential cmake libusb-1.0 libusb-1.0-0-dev
	[ -d "$(ROOTDIR)/stlink-repo/.git" ] || git clone https://github.com/texane/stlink.git "$(ROOTDIR)/stlink-repo"
	[ -d "$(ROOTDIR)/stlink-build" ] || mkdir -p "$(ROOTDIR)/stlink-build"
	{ umask 022; cd "$(ROOTDIR)/stlink-repo" && patch -p1 < "$(ROOTDIR)/patches/stlink_L412.patch" ; }
	{ umask 022; cd "$(ROOTDIR)/stlink-build" && cmake -DCMAKE_BUILD_TYPE=Release ../stlink-repo && make -j$(shell nproc) && sudo make install && sudo ldconfig ; }
	rm -rf "$(ROOTDIR)/stlink-build" "$(ROOTDIR)/stlink-repo"
