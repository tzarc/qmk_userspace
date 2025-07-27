# Copyright 2018-2024 Nick Brassel (@tzarc)
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
	git clone --depth=1 https://github.com/tzarc/qmk_firmware.git $(QMK_USERSPACE)/qmk_firmware \
		&& cd $(QMK_USERSPACE)/qmk_firmware \
		&& git sshsign

$(QMK_USERSPACE)/qmk_userspace:
	git clone --depth=1 https://github.com/qmk/qmk_userspace.git $(QMK_USERSPACE)/qmk_userspace \
		&& cd $(QMK_USERSPACE)/qmk_userspace \
		&& git pull --ff-only \
		&& git sshsign

$(QMK_USERSPACE)/qmk-dot-github:
	git clone --depth=1 https://github.com/qmk/.github.git $(QMK_USERSPACE)/qmk-dot-github \
		&& cd $(QMK_USERSPACE)/qmk-dot-github \
		&& git pull --ff-only \
		&& git sshsign

$(QMK_USERSPACE)/qmk_base_container:
	git clone --depth=1 https://github.com/qmk/qmk_base_container.git $(QMK_USERSPACE)/qmk_base_container \
		&& cd $(QMK_USERSPACE)/qmk_base_container \
		&& git pull --ff-only \
		&& git sshsign

$(QMK_USERSPACE)/qmk_cli:
	git clone --depth=1 https://github.com/qmk/qmk_cli.git $(QMK_USERSPACE)/qmk_cli \
		&& cd $(QMK_USERSPACE)/qmk_cli \
		&& git pull --ff-only \
		&& git sshsign

$(QMK_USERSPACE)/qmk_compiler:
	git clone --depth=1 https://github.com/qmk/qmk_compiler.git $(QMK_USERSPACE)/qmk_compiler \
		&& cd $(QMK_USERSPACE)/qmk_compiler \
		&& git pull --ff-only \
		&& git sshsign

qmk_firmware: $(QMK_USERSPACE)/qmk_firmware
qmk_userspace: $(QMK_USERSPACE)/qmk_userspace
qmk-dot-github: $(QMK_USERSPACE)/qmk-dot-github
qmk_base_container: $(QMK_USERSPACE)/qmk_base_container
qmk_cli: $(QMK_USERSPACE)/qmk_cli
qmk_compiler: $(QMK_USERSPACE)/qmk_compiler

.PHONY: repositories
repositories: qmk_firmware qmk_userspace qmk-dot-github qmk_base_container qmk_cli qmk_compiler

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# CLI testing

.PHONY: cli_build
cli_build: qmk_cli qmk_firmware
	uv pip install -r $(QMK_USERSPACE)/qmk_firmware/requirements-dev.txt \
		&& uv pip install -e $(QMK_USERSPACE)/qmk_cli

.PHONY: cli_reinstall
cli_reinstall:
	uv pip install qmk --force-reinstall \
		&& post-checkout.sh

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Cleaning

.PHONY: clean distclean userspace_clean

userspace_clean:
	@rm -f $(QMK_USERSPACE)/compile_commands.json $(QMK_USERSPACE)/*.bin $(QMK_USERSPACE)/*.hex $(QMK_USERSPACE)/*.uf2 || true
	@rm -rf $(QMK_USERSPACE)/.cache || true

clean: userspace_clean
	@$(MAKE) -C $(QMK_USERSPACE)/qmk_firmware clean

distclean: userspace_clean
	@$(MAKE) -C $(QMK_USERSPACE)/qmk_firmware distclean

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Container management

CONTAINER_PREAMBLE := export HOME="/tmp"; export PATH="/tmp/.local/bin:\$$PATH"; python3 -m pip install --upgrade pip; python3 -m pip install -r requirements-dev.txt

.PHONY: format-core pytest format-and-pytest
format-core:
	make -C $(QMK_USERSPACE)/qmk_firmware format-core

pytest:
	make -C $(QMK_USERSPACE)/qmk_firmware pytest

format-and-pytest:
	make -C $(QMK_USERSPACE)/qmk_firmware format-and-pytest

.PHONY: format-keyboards-tzarc
format-keyboards-tzarc:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c \$$(find keyboards/tzarc \( -name '*.c' -o -name '*.h' \) -and -not -name '*.qgf.*' -and -not -name '*.qff.*' -and -not -name '*conf.h' -and -not -name 'board.h')"

.PHONY: all-vusb all-lufa all-chibios all-riot
all-vusb:
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || $(ECHO) 2) -f protocol=VUSB

all-lufa:
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || $(ECHO) 2) -f protocol=LUFA

all-chibios:
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || $(ECHO) 2) -f protocol=ChibiOS

all-riot:
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || $(ECHO) 2) -f protocol=RIOT

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Helpers

.PHONY: rgb_effects generated-files format env-info

generated-files: rgb_effects

rgb_effects:
	@"$(QMK_USERSPACE)/bin/generate_rgb_effects.py" > "$(QMK_USERSPACE)/users/tzarc/enable_all_rgb_effects.h"

format:
	@git ls-files | grep -E '\.(c|h|cpp|hpp|cxx|hxx|inc|inl)$$' | grep -vE '\.q[gf]f\.' | grep -vE '(ch|hal|mcu)conf\.h$$' | grep -vE 'board.[ch]$$' | grep -vE '.inl.h$$' | grep -vE 'mini-rv32ima.h$$' | while read file ; do \
		$(ECHO) -e "\e[38;5;14mFormatting: $$file\e[0m" ; \
		clang-format -i "$$file" ; \
	done
	@git ls-files | grep -E 'key(board|map)\.json' | while read file ; do \
		$(ECHO) -e "\e[38;5;14mFormatting: $$file\e[0m" ; \
		qmk format-json -i "$$file" ; \
	done
	@git ls-files | grep -E '\.py$$' | while read file ; do \
		$(ECHO) -e "\e[38;5;14mFormatting: $$file\e[0m" ; \
		ruff format --quiet --line-length 160 "$$file" ; \
	done
	@make -C $(QMK_USERSPACE)/modules/tzarc

format-qmk-keyboards-tzarc:
	@git -C $(QMK_USERSPACE)/qmk_firmware ls-files | grep tzarc | grep -E '\.(c|h|cpp|hpp|cxx|hxx)$$' | grep -vE '\.q[gf]f\.' | grep -vE '(ch|hal|mcu)conf\.h$$' | grep -vE 'board.[ch]$$' | grep -vE 'mini-rv32ima.h$$' | while read file ; do \
		$(ECHO) -e "\e[38;5;14mFormatting: $$file\e[0m" ; \
		clang-format -i "$(QMK_USERSPACE)/qmk_firmware/$$file" ; \
	done

env-info:
	@echo
	@env | sort
	@echo
	@command -v avr-gcc
	@avr-gcc -v
	@echo
	@command -v arm-none-eabi-gcc
	@arm-none-eabi-gcc -v
	@echo
	@command -v riscv32-unknown-elf-gcc
	@riscv32-unknown-elf-gcc -v
	@echo

.PHONY: links link-hooks

links: link-hooks

link-hooks:
	@[ -e "$(QMK_USERSPACE)/qmk_firmware/.git/hooks/post-checkout" ] || \
		{ \
			$(ECHO) -e "\e[38;5;14mSymlinking: $(QMK_USERSPACE)/bin/post-checkout.sh -> $(QMK_USERSPACE)/qmk_firmware/.git/hooks/post-checkout\e[0m" ; \
			ln -sf "$(QMK_USERSPACE)/bin/post-checkout.sh" "$(QMK_USERSPACE)/qmk_firmware/.git/hooks/post-checkout" ; \
		}

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Builds

# short name ! keyboard path ! keymap name
BOARD_DEFS := \
	annepro2!annepro2/c18!tzarc \
	bm16s!kprepublic/bm16s!tzarc \
	bm60!kprepublic/bm60hsrgb_poker/rev2!tzarc \
	chocopad!keebio/chocopad!tzarc \
	chocv!chocv!tzarc \
	ctrl!massdrop/ctrl!tzarc \
	djinn_rev1!tzarc/djinn/rev1!tzarc \
	djinn_rev2!tzarc/djinn/rev2!tzarc \
	ghoul_stm32!tzarc/ghoul/rev1/stm32!tzarc \
	ghoul_rp2040!tzarc/ghoul/rev1/rp2040!tzarc \
	geekboards!geekboards/macropad_v2!tzarc \
	iris!keebio/iris/rev4!tzarc \
	jj40!kprepublic/jj40!tzarc \
	kobold!tzarc/kobold/r1!tzarc \
	luddite!40percentclub/luddite!tzarc \
	mysterium!coseyfannitutti/mysterium!tzarc \
	mysterium-dad!coseyfannitutti/mysterium!dad \
	one2mini!ducky/one2mini/1861st!tzarc \
	planck!planck/rev6_drop!tzarc \
	sat75!cannonkeys/satisfaction75/rev1!tzarc \
	prime_plus!keyboards/steelseries/prime_plus!tzarc

# short name ! keyboard path ! keymap name ! link source ! link target
LINKED_BOARD_DEFS := \
	cyclone!keyboards/tzarc/cyclone!default!development/keyboard-dev/tzarc-cyclone \
	\
	evqwk_f401_tinyuf2!keyboards/tzarc/evqwk_f401_tinyuf2!default!development/alternates/evqwk_f401_tinyuf2 \
	weact_h723core!keyboards/handwired/onekey/weact_h723core!console!development/alternates/weact_h723core \
	f411_tinyuf2_filesystem!keyboards/handwired/onekey/f411_tinyuf2_filesystem!reset!development/alternates/f411_tinyuf2_filesystem \
	disco_f723!keyboards/handwired/onekey/disco_f723!console!development/alternates/disco_f723 \
	onekey_h743!keyboards/handwired/onekey/nucleo144_h743zi!console!development/alternates/nucleo144_h743zi \
	onekey_l152!keyboards/handwired/onekey/nucleo64_l152re!console!development/alternates/nucleo64_l152re \
	onekey_l082!keyboards/handwired/onekey/nucleo32_l082!console!development/alternates/nucleo32_l082kz \
	split_l082!keyboards/handwired/splittest/nucleo32_l082!default!development/alternates/nucleo32_l082kz_split \
	test_proton_c!keyboards/handwired/onekey/proton_c_test!console!development/alternates/proton_c_test \
	split_proton_c!keyboards/handwired/splittest/proton_c_split/a9!default!development/alternates/proton_c_split!handwired/splittest/proton_c_split \
	split_g431!keyboards/handwired/splittest/g431_split!default!development/alternates/g431_split \
	split_f072!keyboards/handwired/splittest/f072_split!default!development/alternates/f072_split \
	split_f401!keyboards/handwired/splittest/f401_split!default!development/alternates/f401_split \
	split_f411!keyboards/handwired/splittest/f411_split!default!development/alternates/f411_split \
	spi_eeprom_promicro!keyboards/handwired/onekey/spi_eeprom_test_promicro!eep_rst!development/alternates/spi_eeprom_test/promicro_test \
	spi_eeprom_proton_c!keyboards/handwired/onekey/spi_eeprom_test_proton_c!eep_rst!development/alternates/spi_eeprom_test/proton_c_test \
	spi_eeprom_f103!keyboards/handwired/onekey/spi_eeprom_test_f103!eep_rst!development/alternates/spi_eeprom_test/f103_test \
	spi_eeprom_f401!keyboards/handwired/onekey/spi_eeprom_test_f401!eep_rst!development/alternates/spi_eeprom_test/f401_test \
	l151x8xxa!keyboards/l151x8xxa!reset!development/alternates/l151x8xxa \
	f072_shiftreg!keyboards/f072_shiftreg!default!development/alternates/f072_shiftreg \
	l412_shiftreg!keyboards/l412_shiftreg!default!development/alternates/l412_shiftreg \
	qp_test!keyboards/qp_test!default!development/alternates/qp_test \
	lvgl_test!keyboards/lvgl_test!default!development/alternates/lvgl_test \
	rp2040_split!keyboards/rp2040_split!default!development/alternates/rp2040_split

keyboards_folder := keyboards/
empty :=
define handle_board_entry
board_name_$1 := $$(word 1,$$(subst !, ,$1))
board_keyboard_$1 := $$(word 2,$$(subst !, ,$1))
board_keyboard_stripped_$1 := $$(subst $$(keyboards_folder),$$(empty),$$(board_keyboard_$1))
board_keymap_$1 := $$(word 3,$$(subst !, ,$1))

.PHONY: $$(board_name_$1) bin_$$(board_name_$1)

$$(board_name_$1): bin_$$(board_name_$1)

bin_$$(board_name_$1): rgb_effects
	@$(ECHO) -e "\e[38;5;14mBuilding: $$(board_keyboard_stripped_$1):$$(board_keymap_$1)\e[0m"
	+@cd "$(QMK_USERSPACE)/qmk_firmware" \
		&& { \
			[ -z "$$(NO_COMPILEDB)" ] \
				&& qmk compile --compiledb -kb $$(board_keyboard_stripped_$1) -km $$(board_keymap_$1) \
				|| qmk compile -kb $$(board_keyboard_stripped_$1) -km $$(board_keymap_$1); \
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
	@if [ ! -L "$(QMK_USERSPACE)/qmk_firmware/$$(link_target_$1)" ] ; then \
		$(ECHO) -e "\e[38;5;14mSymlinking: $$(link_source_$1) -> $$(link_target_$1)\e[0m" ; \
		mkdir -p $$(shell dirname "$(QMK_USERSPACE)/qmk_firmware/$$(link_target_$1)") ; \
		ln -sf $(QMK_USERSPACE)/$$(link_source_$1) $(QMK_USERSPACE)/qmk_firmware/$$(link_target_$1) ; \
		if [ -z "$$(grep -P '^$$(link_target_$1)$$$$' $(QMK_USERSPACE)/qmk_firmware/.git/info/exclude 2>/dev/null)" ] ; then \
			$(ECHO) $$(link_target_$1) >> $(QMK_USERSPACE)/qmk_firmware/.git/info/exclude ; \
		fi ; \
	fi

unlinks: unlink_$$(link_target_$1)
unlink_$$(link_target_$1): qmk_firmware
	@if [ -L "$(QMK_USERSPACE)/qmk_firmware/$$(link_target_$1)" ] ; then \
		$(ECHO) -e "\e[38;5;14mRemoving symlink: $$(link_source_$1) -> $$(link_target_$1)\e[0m" ; \
		rm $(QMK_USERSPACE)/qmk_firmware/$$(link_target_$1) || true; \
		$(SED) -i "\@^$$(link_target_$1)@d" $(QMK_USERSPACE)/qmk_firmware/.git/info/exclude; \
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
