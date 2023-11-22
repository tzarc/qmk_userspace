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
# Helpers

.PHONY: rgb_effects

rgb_effects:
	"$(QMK_USERSPACE)/bin/generate_rgb_effects.py" > "$(QMK_USERSPACE)/users/tzarc/enable_all_rgb_effects.h"

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Container management

CONTAINER_PREAMBLE := export HOME="/tmp"; export PATH="/tmp/.local/bin:\$$PATH"; python3 -m pip install --upgrade pip; python3 -m pip install -r requirements-dev.txt

format-core:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c --core-only -a && qmk format-python -a"

pytest:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk pytest"

format-and-pytest:
	cd $(QMK_USERSPACE)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c --core-only -a && qmk format-python -a && qmk pytest"

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Builds

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


define handle_board_entry
board_name_$1 := $$(word 1,$$(subst !, ,$1))
board_keyboard_$1 := $$(word 2,$$(subst !, ,$1))
board_keymap_$1 := $$(word 3,$$(subst !, ,$1))

board_files_$1 := $$(shell find $$(QMK_USERSPACE)/keyboards/$$(board_source_$1) -type f \( -name '*.h' -or -name '*.c' \) -and -not -name '*conf.h' -and -not -name 'board.c' -and -not -name 'board.h' | sort)
board_files_all_$1 := $$(shell find $$(QMK_USERSPACE)/keyboards/$$(board_source_$1) -type f | sort)

.PHONY: bin_$$(board_name_$1)
bin_$$(board_name_$1): rgb_effects
	@$(ECHO) -e "\e[38;5;14mBuilding: $$(board_keyboard_$1):$$(board_keymap_$1)\e[0m"
	+@cd "$(QMK_USERSPACE)/qmk_firmware" \
		&& { [ -z "$$(NO_COMPILEDB)" ] && qmk generate-compilation-database -kb $$(board_keyboard_$1) -km $$(board_keymap_$1) || true; } \
		&& { \
			$$(MAKE) --no-print-directory -r -R -C "$(QMK_USERSPACE)/qmk_firmware" -f "$(QMK_USERSPACE)/qmk_firmware/builddefs/build_keyboard.mk" $$(MAKEFLAGS) KEYBOARD="$$(board_keyboard_$1)" KEYMAP="$$(board_keymap_$1)" REQUIRE_PLATFORM_KEY= COLOR=true SILENT=false QMK_USERSPACE=$(QMK_USERSPACE) ; \
		}
	@[ -e "$$(QMK_USERSPACE)/qmk_firmware/compile_commands.json" ] && cp $$(QMK_USERSPACE)/qmk_firmware/compile_commands.json $$(QMK_USERSPACE) \
		|| true

.PHONY: $$(board_name_$1)
$$(board_name_$1): bin_$$(board_name_$1)

.PHONY: bin
bin: bin_$$(board_name_$1)
endef
$(foreach board_entry,$(BOARD_DEFS),$(eval $(call handle_board_entry,$(board_entry))))

# If we're doing a bulk compilation, don't need compiledb's
bin: NO_COMPILEDB=true

.PHONY: djinn
djinn: NO_COMPILEDB=true
djinn: djinn_rev1 djinn_rev2

.PHONY: ghoul
ghoul: NO_COMPILEDB=true
ghoul: ghoul_stm32 ghoul_rp2040
