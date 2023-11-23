# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# avoid 'Entering|Leaving directory' messages
MAKEFLAGS += --no-print-directory

QMK_USERSPACE := $(patsubst %/,%,$(dir $(shell realpath "$(lastword $(MAKEFILE_LIST))")))
ifeq ($(QMK_USERSPACE),)
    QMK_USERSPACE := $(shell pwd)
endif

# Add extra binaries to path
export PATH := $(QMK_USERSPACE)/bin:$(PATH)

# Deal with macOS
ifeq ($(shell uname -s),Darwin)
SED = gsed
ECHO = gecho
else
SED = sed
ECHO = echo
endif

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Repositories

$(QMK_USERSPACE)/qmk_firmware:
	git clone --depth=1 https://github.com/tzarc/qmk_firmware.git $(QMK_USERSPACE)/qmk_firmware

$(QMK_USERSPACE)/qmk_userspace:
	git clone --depth=1 https://github.com/qmk/qmk_userspace.git $(QMK_USERSPACE)/qmk_userspace

$(QMK_USERSPACE)/qmk-dot-github:
	git clone --depth=1 https://github.com/qmk/.github.git $(QMK_USERSPACE)/qmk-dot-github

qmk_firmware: $(QMK_USERSPACE)/qmk_firmware
qmk_userspace: $(QMK_USERSPACE)/qmk_userspace
qmk-dot-github: $(QMK_USERSPACE)/qmk-dot-github

.PHONY: repositories
repositories: qmk_firmware qmk_userspace qmk-dot-github

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Cleaning

.PHONY: clean distclean userspace_clean

userspace_clean:
	@rm -f $(QMK_USERSPACE)/compile_commands.json || true

clean: userspace_clean
	@$(MAKE) -C $(QMK_USERSPACE)/qmk_firmware clean

distclean: userspace_clean
	@$(MAKE) -C $(QMK_USERSPACE)/qmk_firmware distclean

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Container management

CONTAINER_PREAMBLE := export HOME="/tmp"; export PATH="/tmp/.local/bin:\$$PATH"; python3 -m pip install --upgrade pip; python3 -m pip install -r requirements-dev.txt

.PHONY: format-core pytest format-and-pytest

format-core:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c --core-only -a && qmk format-python -a"

pytest:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk pytest"

format-and-pytest:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c --core-only -a && qmk format-python -a && qmk pytest"


all-chibios:
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || $(ECHO) 2) -f protocol=ChibiOS

all-riot:
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || $(ECHO) 2) -f protocol=RIOT

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Helpers

.PHONY: qmk_firmware rgb_effects generated-files

qmk_firmware:

generated-files: rgb_effects

rgb_effects:
	"$(QMK_USERSPACE)/bin/generate_rgb_effects.py" > "$(QMK_USERSPACE)/users/tzarc/enable_all_rgb_effects.h"

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Builds

# short name ! keyboard path ! keymap name
BOARD_DEFS := \
	annepro2!annepro2/c18!tzarc \
	bm16s!kprepublic/bm16s!tzarc \
	bm60!kprepublic/bm60hsrgb_poker/rev2!tzarc \
	chocopad!keebio/chocopad!tzarc \
	ctrl!massdrop/ctrl!tzarc \
	djinn_rev1!tzarc/djinn/rev1!tzarc \
	djinn_rev2!tzarc/djinn/rev2!tzarc \
	ghoul_stm32!tzarc/ghoul/rev1/stm32!tzarc \
	ghoul_rp2040!tzarc/ghoul/rev1/rp2040!tzarc \
	geekboards!geekboards/macropad_v2!tzarc \
	iris!keebio/iris/rev4!tzarc \
	jj40!kprepublic/jj40!tzarc \
	luddite!40percentclub/luddite!tzarc \
	mysterium!coseyfannitutti/mysterium!tzarc \
	mysterium-dad!coseyfannitutti/mysterium!dad \
	one2mini!ducky/one2mini/1861st!tzarc \
	planck!planck/rev6_drop!tzarc \
	sat75!cannonkeys/satisfaction75/rev1!tzarc

# short name ! keyboard path ! keymap name ! link source ! link target
LINKED_BOARD_DEFS := \
	disco_f723!handwired/onekey/disco_f723!console!alternates/disco_f723 \
	onekey_h743!handwired/onekey/nucleo144_h743zi!console!alternates/nucleo144_h743zi \
	onekey_l152!handwired/onekey/nucleo64_l152re!console!alternates/nucleo64_l152re \
	onekey_l082!handwired/onekey/nucleo32_l082!console!alternates/nucleo32_l082kz \
	split_l082!handwired/splittest/nucleo32_l082!default!alternates/nucleo32_l082kz_split \
	test_proton_c!handwired/onekey/proton_c_test!console!alternates/proton_c_test \
	split_proton_c!handwired/splittest/proton_c_split/a9!default!alternates/proton_c_split!handwired/splittest/proton_c_split \
	split_g431!handwired/splittest/g431_split!default!alternates/g431_split \
	split_f072!handwired/splittest/f072_split!default!alternates/f072_split \
	split_f401!handwired/splittest/f401_split!default!alternates/f401_split \
	split_f411!handwired/splittest/f411_split!default!alternates/f411_split \
	spi_eeprom_promicro!handwired/onekey/spi_eeprom_test_promicro!eep_rst!alternates/spi_eeprom_test/promicro_test \
	spi_eeprom_proton_c!handwired/onekey/spi_eeprom_test_proton_c!eep_rst!alternates/spi_eeprom_test/proton_c_test \
	spi_eeprom_f103!handwired/onekey/spi_eeprom_test_f103!eep_rst!alternates/spi_eeprom_test/f103_test \
	spi_eeprom_f401!handwired/onekey/spi_eeprom_test_f401!eep_rst!alternates/spi_eeprom_test/f401_test \
	l151x8xxa!l151x8xxa!reset!alternates/l151x8xxa \
	f072_shiftreg!f072_shiftreg!default!alternates/f072_shiftreg \
	l412_shiftreg!l412_shiftreg!default!alternates/l412_shiftreg \
	qp_test!qp_test!default!alternates/qp_test \
	lvgl_test!lvgl_test!default!alternates/lvgl_test

define handle_board_entry
board_name_$1 := $$(word 1,$$(subst !, ,$1))
board_keyboard_$1 := $$(word 2,$$(subst !, ,$1))
board_keymap_$1 := $$(word 3,$$(subst !, ,$1))

board_files_$1 := $$(shell find $$(QMK_USERSPACE)/keyboards/$$(board_source_$1) -type f \( -name '*.h' -or -name '*.c' \) -and -not -name '*conf.h' -and -not -name 'board.c' -and -not -name 'board.h' | sort)
board_files_all_$1 := $$(shell find $$(QMK_USERSPACE)/keyboards/$$(board_source_$1) -type f | sort)

.PHONY: $$(board_name_$1) bin_$$(board_name_$1)

$$(board_name_$1): bin_$$(board_name_$1)

bin_$$(board_name_$1): rgb_effects
	@$(ECHO) -e "\e[38;5;14mBuilding: $$(board_keyboard_$1):$$(board_keymap_$1)\e[0m"
	+@cd "$(QMK_USERSPACE)/qmk_firmware" \
		&& { [ -z "$$(NO_COMPILEDB)" ] && qmk generate-compilation-database -kb $$(board_keyboard_$1) -km $$(board_keymap_$1) || true; } \
		&& { \
			$$(MAKE) --no-print-directory -r -R -C "$(QMK_USERSPACE)/qmk_firmware" -f "$(QMK_USERSPACE)/qmk_firmware/builddefs/build_keyboard.mk" $$(MAKEFLAGS) KEYBOARD="$$(board_keyboard_$1)" KEYMAP="$$(board_keymap_$1)" REQUIRE_PLATFORM_KEY= COLOR=true SILENT=false QMK_USERSPACE=$(QMK_USERSPACE) ; \
		}
	@[ -e "$$(QMK_USERSPACE)/qmk_firmware/compile_commands.json" ] && cp $$(QMK_USERSPACE)/qmk_firmware/compile_commands.json $$(QMK_USERSPACE) \
		|| true
endef
$(foreach board_entry,$(BOARD_DEFS),$(eval $(call handle_board_entry,$(board_entry))))

define handle_linked_board_entry
board_name_$1 := $$(word 1,$$(subst !, ,$1))
board_keyboard_$1 := $$(word 2,$$(subst !, ,$1))
board_keymap_$1 := $$(word 3,$$(subst !, ,$1))
link_source_$1 := $$(word 4,$$(subst !, ,$1))
link_target_$1 := $$(word 5,$$(subst !, ,$1))

ifeq ($$(link_target_$1),)
link_target_$1 := $$(board_keyboard_$1)
endif

bin_$$(board_name_$1): link_$$(link_target_$1)

.PHONY: links link_$$(link_target_$1) unlinks unlink_$$(link_target_$1)

links: link_$$(link_target_$1)
link_$$(link_target_$1): qmk_firmware
	@if [ ! -L "$(QMK_USERSPACE)/qmk_firmware/keyboards/$$(link_target_$1)" ] ; then \
		$(ECHO) -e "\e[38;5;14mSymlinking: $$(link_source_$1) -> $$(link_target_$1)\e[0m" ; \
		ln -sf $(QMK_USERSPACE)/$$(link_source_$1) $(QMK_USERSPACE)/qmk_firmware/keyboards/$$(link_target_$1) ; \
		if [ -z "$$(grep -P '^keyboards/$$(link_target_$1)$$$$' $(QMK_USERSPACE)/qmk_firmware/.git/info/exclude 2>/dev/null)" ] ; then \
			$(ECHO) keyboards/$$(link_target_$1) >> $(QMK_USERSPACE)/qmk_firmware/.git/info/exclude ; \
		fi ; \
	fi

unlinks: unlink_$$(link_target_$1)
unlink_$$(link_target_$1): qmk_firmware
	@if [ -L "$(QMK_USERSPACE)/qmk_firmware/keyboards/$$(link_target_$1)" ] ; then \
		$(ECHO) -e "\e[38;5;14mRemoving symlink: $$(link_source_$1) -> $$(link_target_$1)\e[0m" ; \
		rm $(QMK_USERSPACE)/qmk_firmware/keyboards/$$(link_target_$1) || true; \
		$(SED) -i "\@^keyboards/$$(link_target_$1)@d" $(QMK_USERSPACE)/qmk_firmware/.git/info/exclude; \
	fi

clean: unlink_$$(link_target_$1)
distclean: unlink_$$(link_target_$1)

generated-files: links
endef
$(foreach board_entry,$(LINKED_BOARD_DEFS),$(eval $(call handle_board_entry,$(board_entry))))
$(foreach board_entry,$(LINKED_BOARD_DEFS),$(eval $(call handle_linked_board_entry,$(board_entry))))

.PHONY: djinn
djinn: NO_COMPILEDB=true
djinn: djinn_rev1 djinn_rev2

.PHONY: ghoul
ghoul: NO_COMPILEDB=true
ghoul: ghoul_stm32 ghoul_rp2040
