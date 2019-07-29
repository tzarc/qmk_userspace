########################################################################################################################
# Common

export ROOTDIR := $(shell pwd)
export PATH := $(HOME)/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$(PATH)

all: bin

update:
	git submodule update --remote
	make -C "$(ROOTDIR)/qmk_firmware" git-submodule

clean:
	rm *.bin *.hex || true
	make -C "$(ROOTDIR)/qmk_firmware" clean || true

distclean:
	rm *.bin *.hex || true
	make -C "$(ROOTDIR)/qmk_firmware" distclean || true

bin: bin_cyclone bin_luddite

format: format_cyclone

########################################################################################################################
# ST-Flash

stlink: /usr/local/bin/st-flash

/usr/local/bin/st-flash:
	$(ROOTDIR)/build_st-flash.sh

########################################################################################################################
# DFU Util

dfu-util: /usr/local/bin/dfu-util

/usr/local/bin/dfu-util:
	$(ROOTDIR)/build_dfu-util.sh

########################################################################################################################
# Cyclone

DEFAULT_CYCLONE = handwired_tzarc_cyclone_revA_default

cyclone: bin_cyclone

remove_cyclone:
	@rm -f "$(ROOTDIR)/handwired_tzarc_cyclone_*.bin" || true

bin_cyclone: handwired_tzarc_cyclone_revA_default.bin

CYCLONE_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/handwired/tzarc/cyclone" -type f)
handwired_tzarc_cyclone_revA_default.bin: remove_cyclone $(CYCLONE_DEPS)
	make -C "$(ROOTDIR)/qmk_firmware" handwired/tzarc/cyclone:default
	cp "$(ROOTDIR)/qmk_firmware/handwired_tzarc_cyclone_revA_default.bin" "$(ROOTDIR)"

bin_cyclone: $(DEFAULT_CYCLONE).bin

dfu_cyclone: dfu-util $(DEFAULT_CYCLONE).bin
	make -C "$(ROOTDIR)/qmk_firmware" handwired/tzarc/cyclone:default:dfu-util

flash_cyclone: stlink $(DEFAULT_CYCLONE).bin
	st-flash erase
	st-flash write "$(ROOTDIR)/$(DEFAULT_CYCLONE).bin" 0x08000000

debug_cyclone: bin_cyclone
	{ cd "$(ROOTDIR)/qmk_firmware" && arm-none-eabi-gdb --command="$(ROOTDIR)/gdbinit" --exec="$(ROOTDIR)/qmk_firmware/.build/$(DEFAULT_CYCLONE).elf" --symbols="$(ROOTDIR)/qmk_firmware/.build/$(DEFAULT_CYCLONE).elf" ; }

dump_cyclone: bin_cyclone
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/$(DEFAULT_CYCLONE).elf"

CYCLONE_COMMON_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/handwired/tzarc/cyclone" -maxdepth 1 -type f \( -name '*.h' -or -name '*.c' \) | sort)
CYCLONE_REVA_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/handwired/tzarc/cyclone/revA" -maxdepth 1 -type f \( -name 'config.h' -or -name '*revA*.h' -or -name '*revA*.c' \) | sort)
format_cyclone:
	@for file in $(CYCLONE_COMMON_FORMATTABLE_FILES) $(CYCLONE_REVA_FORMATTABLE_FILES); do \
		echo "clang-format $${file}..." ; \
		clang-format -i "$$file" ; \
		dos2unix "$$file" ; \
	done

########################################################################################################################
# Luddite

luddite: bin_luddite

remove_luddite:
	@rm -f "$(ROOTDIR)"/40percentclub_luddite_tzarc*.hex || true

bin_luddite: 40percentclub_luddite_tzarc.hex

LUDDITE_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/40percentclub/luddite/keymaps/tzarc" -type f)
40percentclub_luddite_tzarc.hex: remove_luddite $(LUDDITE_DEPS)
	make -C "$(ROOTDIR)/qmk_firmware" 40percentclub/luddite:tzarc:production
	cp "$(ROOTDIR)/qmk_firmware"/40percentclub_luddite_tzarc*.hex "$(ROOTDIR)"

bin_luddite: 40percentclub_luddite_tzarc.hex

dfu_luddite: dfu-util 40percentclub_luddite_tzarc.hex
	make -C "$(ROOTDIR)/qmk_firmware" 40percentclub/luddite:tzarc:dfu

dump_luddite: bin_luddite
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/40percentclub_luddite_tzarc.elf"
