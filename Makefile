########################################################################################################################
# Common

export ROOTDIR := $(shell pwd)

ifeq '$(findstring ;,$(PATH))' ';'
    detected_OS := Windows
else
    detected_OS := $(shell uname 2>/dev/null || echo Unknown)
    detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
    detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
    detected_OS := $(patsubst MINGW%,MSYS,$(detected_OS))
endif

all: bin

update:
	git submodule update --remote
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" git-submodule

clean:
	rm *.bin *.hex *.dump || true
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean || true

distclean:
	rm *.bin *.hex *.dump || true
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" distclean || true

bin: cyclone iris luddite chocopad

format: format_boards format_eeprom_driver

########################################################################################################################
# Symlinks (Linux/Mac)

ifeq ($(detected_OS),$(filter Linux Darwin,$(detected_OS)))

links:
	[ -e $(ROOTDIR)/.clang-format ] || \
		ln -sf $(ROOTDIR)/qmk_firmware/.clang-format $(ROOTDIR)/.clang-format
	[ -e $(ROOTDIR)/tzarc-cyclone ] || \
		ln -sf $(ROOTDIR)/qmk_firmware/keyboards/handwired/tzarc/cyclone $(ROOTDIR)/tzarc-cyclone
	[ -e $(ROOTDIR)/tzarc-cyclone-default ] || \
		ln -sf $(ROOTDIR)/qmk_firmware/keyboards/handwired/tzarc/cyclone/keymaps/default $(ROOTDIR)/tzarc-cyclone-default
	[ -e $(ROOTDIR)/tzarc-chocopad ] || \
		ln -sf $(ROOTDIR)/qmk_firmware/keyboards/keebio/chocopad/keymaps/tzarc $(ROOTDIR)/tzarc-chocopad
	[ -e $(ROOTDIR)/tzarc-iris_rev4 ] || \
		ln -sf $(ROOTDIR)/qmk_firmware/keyboards/keebio/iris/keymaps/tzarc $(ROOTDIR)/tzarc-iris_rev4
	[ -e $(ROOTDIR)/tzarc-luddite ] || \
		ln -sf $(ROOTDIR)/qmk_firmware/keyboards/40percentclub/luddite/keymaps/tzarc $(ROOTDIR)/tzarc-luddite

else

links:
	@echo Unknown OS: $(detected_OS)

endif

########################################################################################################################
# Tests

test_eeprom_builds:
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/promicro:default
	avr-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_promicro_default.elf" > "$(ROOTDIR)/handwired_onekey_promicro_default.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/promicro:default EEPROM_DRIVER=vendor
	avr-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_promicro_default.elf" > "$(ROOTDIR)/handwired_onekey_promicro_default--vendor.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/promicro:default EEPROM_DRIVER=i2c
	avr-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_promicro_default.elf" > "$(ROOTDIR)/handwired_onekey_promicro_default--i2c.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/promicro:default EEPROM_DRIVER=transient
	avr-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_promicro_default.elf" > "$(ROOTDIR)/handwired_onekey_promicro_default--transient.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/proton_c:default
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_proton_c_default.elf" > "$(ROOTDIR)/handwired_onekey_proton_c_default.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/proton_c:default EEPROM_DRIVER=vendor
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_proton_c_default.elf" > "$(ROOTDIR)/handwired_onekey_proton_c_default--vendor.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/proton_c:default EEPROM_DRIVER=i2c
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_proton_c_default.elf" > "$(ROOTDIR)/handwired_onekey_proton_c_default--i2c.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/onekey/proton_c:default EEPROM_DRIVER=transient
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/handwired_onekey_proton_c_default.elf" > "$(ROOTDIR)/handwired_onekey_proton_c_default--transient.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" massdrop/alt:default
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/massdrop_alt_default.elf" > "$(ROOTDIR)/massdrop_alt_default.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" massdrop/alt:default EEPROM_DRIVER=vendor
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/massdrop_alt_default.elf" > "$(ROOTDIR)/massdrop_alt_default--vendor.dump"

	#$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	#$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" massdrop/alt:default EEPROM_DRIVER=i2c
	#arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/massdrop_alt_default.elf" > "$(ROOTDIR)/massdrop_alt_default--i2c.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" massdrop/alt:default EEPROM_DRIVER=transient
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/massdrop_alt_default.elf" > "$(ROOTDIR)/massdrop_alt_default--transient.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" ergodox_infinity:default
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/ergodox_infinity_default.elf" > "$(ROOTDIR)/ergodox_infinity_default.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" ergodox_infinity:default EEPROM_DRIVER=vendor
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/ergodox_infinity_default.elf" > "$(ROOTDIR)/ergodox_infinity_default--vendor.dump"

	#$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	#$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" ergodox_infinity:default EEPROM_DRIVER=i2c
	#arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/ergodox_infinity_default.elf" > "$(ROOTDIR)/ergodox_infinity_default--i2c.dump"

	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" ergodox_infinity:default EEPROM_DRIVER=transient
	arm-none-eabi-objdump -d "$(ROOTDIR)/qmk_firmware/.build/ergodox_infinity_default.elf" > "$(ROOTDIR)/ergodox_infinity_default--transient.dump"

########################################################################################################################
# Common

EEPROM_DRIVER_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)/qmk_firmware/drivers/eeprom" -maxdepth 1 -type f \( -name '*.h' -or -name '*.c' \) | sort)

format_eeprom_driver:
	@for file in $(EEPROM_DRIVER_FORMATTABLE_FILES) ; do \
		echo "formatting $${file}..." ; \
		clang-format -i "$$file" ; \
		dos2unix "$$file" ; \
	done

BOARD_COMMON_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)"/tzarc*/ -maxdepth 1 -type f \( -name '*.h' -or -name '*.c' \) -and -not -name '*conf.h' | sort)
format_boards:
	@for file in $(BOARD_COMMON_FORMATTABLE_FILES) ; do \
		echo "formatting $${file}..." ; \
		clang-format -i "$$file" ; \
		dos2unix "$$file" ; \
	done

########################################################################################################################
# Cyclone

DEFAULT_CYCLONE = handwired_tzarc_cyclone_default

cyclone: bin_cyclone

remove_cyclone:
	@rm -f "$(ROOTDIR)/handwired_tzarc_cyclone_*.bin" || true

bin_cyclone: handwired_tzarc_cyclone_default.bin

flash_cyclone: $(DEFAULT_CYCLONE).bin
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/tzarc/cyclone:default:flash

debug_cyclone: bin_cyclone
	{ cd "$(ROOTDIR)/qmk_firmware" && arm-none-eabi-gdb --command="$(ROOTDIR)/gdbinit" --exec="$(ROOTDIR)/qmk_firmware/.build/$(DEFAULT_CYCLONE).elf" --symbols="$(ROOTDIR)/qmk_firmware/.build/$(DEFAULT_CYCLONE).elf" ; }

dump_cyclone: bin_cyclone
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/$(DEFAULT_CYCLONE).elf"

CYCLONE_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/handwired/tzarc/cyclone" -type f)
handwired_tzarc_cyclone_default.bin: remove_cyclone $(CYCLONE_DEPS)
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" handwired/tzarc/cyclone:default __CFLAGS+="-Wcast-align"
	cp "$(ROOTDIR)/qmk_firmware/handwired_tzarc_cyclone_default.bin" "$(ROOTDIR)"

########################################################################################################################
# Iris rev4

iris: bin_iris

remove_iris:
	@rm -f "$(ROOTDIR)"/keebio_iris_rev4*.hex || true

bin_iris: keebio_iris_rev4_tzarc.hex

boot_iris: keebio_iris_rev4_tzarc_production.hex

flash_iris: keebio_iris_rev4_tzarc.hex
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" keebio/iris/rev4:tzarc:flash

dump_iris: bin_iris
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/keebio_iris_rev4_tzarc.elf"

IRIS_REV4_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/keebio/iris/keymaps/tzarc" -type f)
keebio_iris_rev4_tzarc.hex: remove_iris $(IRIS_REV4_DEPS)
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" keebio/iris/rev4:tzarc
	cp "$(ROOTDIR)/qmk_firmware"/keebio_iris_rev4_tzarc*.hex "$(ROOTDIR)"

keebio_iris_rev4_tzarc_production.hex: remove_iris $(IRIS_REV4_DEPS)
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" keebio/iris/rev4:tzarc:production
	cp "$(ROOTDIR)/qmk_firmware"/keebio_iris_rev4_tzarc*.hex "$(ROOTDIR)"

########################################################################################################################
# Luddite

luddite: bin_luddite

remove_luddite:
	@rm -f "$(ROOTDIR)"/40percentclub_luddite_tzarc*.hex || true

bin_luddite: 40percentclub_luddite_tzarc.hex

boot_luddite: 40percentclub_luddite_tzarc_production.hex

flash_luddite: 40percentclub_luddite_tzarc.hex
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" 40percentclub/luddite:tzarc:flash

dump_luddite: bin_luddite
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/40percentclub_luddite_tzarc.elf"

LUDDITE_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/40percentclub/luddite/keymaps/tzarc" -type f)
40percentclub_luddite_tzarc.hex: remove_luddite $(LUDDITE_DEPS)
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" 40percentclub/luddite:tzarc
	cp "$(ROOTDIR)/qmk_firmware"/40percentclub_luddite_tzarc*.hex "$(ROOTDIR)"

40percentclub_luddite_tzarc_production.hex: remove_luddite $(LUDDITE_DEPS)
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" 40percentclub/luddite:tzarc:production
	cp "$(ROOTDIR)/qmk_firmware"/40percentclub_luddite_tzarc*.hex "$(ROOTDIR)"

########################################################################################################################
# Chocopad

chocopad: bin_chocopad

remove_chocopad:
	@rm -f "$(ROOTDIR)"/keebio_chocopad_default*.hex || true

bin_chocopad: keebio_chocopad_default.hex

boot_chocopad: keebio_chocopad_default_production.hex

flash_chocopad: dfu-util keebio_chocopad_default.hex
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" keebio/chocopad:default:flash

dump_chocopad: bin_chocopad
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/keebio_chocopad_default.elf"

CHOCOPAD_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/keebio/chocopad/keymaps/default" -type f)
keebio_chocopad_default.hex: remove_chocopad $(CHOCOPAD_DEPS)
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" keebio/chocopad:default
	cp "$(ROOTDIR)/qmk_firmware"/keebio_chocopad_default*.hex "$(ROOTDIR)"

keebio_chocopad_default_production.hex: remove_chocopad $(CHOCOPAD_DEPS)
	$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" keebio/chocopad:default:production
	cp "$(ROOTDIR)/qmk_firmware"/keebio_chocopad_default*.hex "$(ROOTDIR)"
