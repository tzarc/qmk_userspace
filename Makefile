# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

export ROOTDIR := $(shell pwd)
#export PATH := /home/nickb/dev/cross-compilers/target_prefix/gcc11.1_arm/bin:$(PATH)
#export PATH := /home/nickb/dev/cross-compilers/target_prefix/gcc11.1_avr/bin:$(PATH)
#export PATH := /home/nickb/gcc-arm/gcc-arm-none-eabi-10.3-2021.10/bin:$(PATH)
#export PATH := /home/nickb/gcc-arm/gcc-arm-none-eabi-10-2020-q4-major/bin:$(PATH)
#export PATH := /home/nickb/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$(PATH)
#export PATH := /usr/lib/ccache:$(PATH)
#export PATH := /home/nickb/qmk-stuff/qmk_toolchains/arm/bin:$(PATH)
#export PATH := /home/nickb/qmk-stuff/qmk_toolchains/avr/bin:$(PATH)

# Add qmk wrapper to path
export PATH := $(ROOTDIR)/bin:$(PATH)

export CLANG_TIDY := $(shell find /usr/lib/llvm* -name 'run-clang-tidy.py' 2>/dev/null)
export CLANG_TIDY_CHECKS := *,-clang-diagnostic-error,-llvm-include-order,-cppcoreguidelines-avoid-non-const-global-variables,-hicpp-braces-around-statements,-readability-braces-around-statements,-google-readability-braces-around-statements,-llvm-header-guard,-bugprone-reserved-identifier,-cert-dcl37-c,-cert-dcl51-cpp,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-clang-diagnostic-ignored-attributes,-clang-diagnostic-unknown-attributes,-misc-unused-parameters,-hicpp-signed-bitwise,-llvmlibc*,-hicpp-uppercase-literal-suffix,-readability-uppercase-literal-suffix,-hicpp-no-assembler
export CLANG_TIDY_HEADER_FILTER := .*

qmk_firmware: $(ROOTDIR)/qmk_firmware

$(ROOTDIR)/qmk_firmware:
	git clone --depth=1 https://github.com/tzarc/qmk_firmware.git $(ROOTDIR)/qmk_firmware

BOARD_DEFS := \
	cyclone!keyboards/tzarc-cyclone!handwired/tzarc/cyclone!tzarc \
	\
	annepro2!keymaps/annepro2!annepro2/c18/keymaps/tzarc!tzarc \
	bm16s!keymaps/bm16s!kprepublic/bm16s/keymaps/tzarc!tzarc \
	bm60!keymaps/bm60hsrgb_poker_rev2!kprepublic/bm60hsrgb_poker/rev2/keymaps/tzarc!tzarc \
	chocopad!keymaps/chocopad!keebio/chocopad/keymaps/tzarc!tzarc \
	ctrl!keymaps/ctrl!massdrop/ctrl/keymaps/tzarc!tzarc \
	djinn_rev1!keymaps/djinn!tzarc/djinn/rev1/keymaps/tzarc!tzarc \
	djinn_rev2!keymaps/djinn!tzarc/djinn/rev2/keymaps/tzarc!tzarc \
	ghoul_stm32!keymaps/ghoul!tzarc/ghoul/rev1/stm32/keymaps/tzarc!tzarc \
	ghoul_rp2040!keymaps/ghoul!tzarc/ghoul/rev1/rp2040/keymaps/tzarc!tzarc \
	geekboards!keymaps/geekboards-macropad_v2!geekboards/macropad_v2/keymaps/tzarc!tzarc \
	iris!keymaps/iris_rev4!keebio/iris/rev4/keymaps/tzarc!tzarc \
	jj40!keymaps/jj40!kprepublic/jj40/keymaps/tzarc!tzarc \
	luddite!keymaps/luddite!40percentclub/luddite/keymaps/tzarc!tzarc \
	mysterium-dad!keymaps/mysterium-dad!coseyfannitutti/mysterium/keymaps/dad!dad \
	mysterium-nick!keymaps/mysterium!coseyfannitutti/mysterium/keymaps/tzarc!tzarc \
	one2mini!keymaps/one2mini!ducky/one2mini/1861st/keymaps/tzarc!tzarc \
	planck!keymaps/planck!planck/rev6_drop/keymaps/tzarc!tzarc \
	sat75!keymaps/sat75!cannonkeys/satisfaction75/rev1/keymaps/tzarc!tzarc \
	\
	disco_f723!alternates/disco_f723!handwired/onekey/disco_f723!console \
	onekey_h723!alternates/nucleo144_h723zg!handwired/onekey/nucleo144_h723zg!console \
	onekey_h743!alternates/nucleo144_h743zi!handwired/onekey/nucleo144_h743zi!console \
	onekey_l152!alternates/nucleo64_l152re!handwired/onekey/nucleo64_l152re!console \
	onekey_l082!alternates/nucleo32_l082kz!handwired/onekey/nucleo32_l082!console \
	split_l082!alternates/nucleo32_l082kz_split!handwired/splittest/nucleo32_l082!default \
	test_proton_c!alternates/proton_c_test!handwired/onekey/proton_c_test!console \
	split_proton_c!alternates/proton_c_split!handwired/splittest/proton_c_split!default!handwired/splittest/proton_c_split/a9 \
	split_g431!alternates/g431_split!handwired/splittest/g431_split!default \
	split_f072!alternates/f072_split!handwired/splittest/f072_split!default \
	split_f401!alternates/f401_split!handwired/splittest/f401_split!default \
	split_f411!alternates/f411_split!handwired/splittest/f411_split!default \
	spi_eeprom_promicro!alternates/spi_eeprom_test/promicro_test!handwired/onekey/spi_eeprom_test_promicro!eep_rst \
	spi_eeprom_proton_c!alternates/spi_eeprom_test/proton_c_test!handwired/onekey/spi_eeprom_test_proton_c!eep_rst \
	spi_eeprom_f103!alternates/spi_eeprom_test/f103_test!handwired/onekey/spi_eeprom_test_f103!eep_rst \
	spi_eeprom_f401!alternates/spi_eeprom_test/f401_test!handwired/onekey/spi_eeprom_test_f401!eep_rst \
	l151x8xxA!alternates/l151x8xxA!l151x8xxA!reset \
	f072_shiftreg!alternates/f072_shiftreg!f072_shiftreg!default \
	l412_shiftreg!alternates/l412_shiftreg!l412_shiftreg!default \
	qp_test!alternates/qp_test!qp_test!default!qp_test \
	lvgl_test!alternates/lvgl_test!lvgl_test!default!lvgl_test

EXTRA_LINK_DEFS := \
	layouts/layout-tkl_ansi!layouts/community/tkl_ansi/tzarc \
	layouts/layout-60_ansi!layouts/community/60_ansi/tzarc \
	layouts/layout-ortho_4x4!layouts/community/ortho_4x4/tzarc \
	layouts/layout-ortho_4x12!layouts/community/ortho_4x12/tzarc \
	users-tzarc!users/tzarc

all-arm:
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2) -f protocol=ChibiOS
	qmk mass-compile -j$(shell getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2) -f protocol=arm_atsam

all: NO_COMPILEDB = true
all: bin

nick: NO_COMPILEDB = true
nick: annepro2 bm16s bm60 chocopad ctrl cyclone djinn geekboards ghoul iris jj40 luddite mysterium-nick one2mini planck sat75

djinn: NO_COMPILEDB = true
djinn: djinn_rev1 djinn_rev2

ghoul: NO_COMPILEDB = true
ghoul: ghoul_stm32 ghoul_rp2040

remove_artifacts: unlinks
	rm -rf "$(ROOTDIR)"/*.bin "$(ROOTDIR)"/*.hex "$(ROOTDIR)"/*.uf2 "$(ROOTDIR)"/*.dump "$(ROOTDIR)"/.clang-format "$(ROOTDIR)"/compile_commands.json "$(ROOTDIR)"/.cache "$(ROOTDIR)"/qmk_firmware/.cache "$(ROOTDIR)"/qmk_firmware/compile_commands.json >/dev/null 2>&1 || true

clean: remove_artifacts
	+$(MAKE) -C "$(ROOTDIR)/qmk_firmware" clean || true

distclean: remove_artifacts
	+$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" distclean || true

format_prereq: qmk_firmware
	@ln -sf $(ROOTDIR)/qmk_firmware/.clang-format $(ROOTDIR)/.clang-format

format: format_prereq
	@for file in $$({ git ls-files | grep -P '\.(c|cpp|h|hpp)$$' ; } | grep -vP 'conf\.h' | grep -vP 'board.h' | sort | uniq) ; do \
		if [ -f "$$file" ] ; then \
			echo -e "\e[38;5;14mFormatting: $$file\e[0m" ; \
			clang-format -i "$$file" >/dev/null 2>&1 || true ; \
			ex -s +"bufdo wq" "$$file" >/dev/null 2>&1 || true ; \
			dos2unix "$$file" >/dev/null 2>&1 ; \
			chmod -x "$$file" >/dev/null 2>&1 ; \
		fi ; \
	done
	@./check-license.sh

links: format_prereq extra-links

git-submodule: qmk_firmware
	+$(MAKE) -C "$(ROOTDIR)/qmk_firmware" git-submodule

graphics:
	+$(MAKE) -C "$(ROOTDIR)/users-tzarc/graphics"
	+$(MAKE) -C "$(ROOTDIR)/alternates/qp_test" -f makefile.graphics

$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h: rgb_effects

rgb_effects:
	@echo '// Copyright 2018-2023 Nick Brassel (@tzarc)' > "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo '// SPDX-License-Identifier: GPL-2.0-or-later' >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo '#pragma once' >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo '#ifdef RGB_MATRIX_ENABLE' >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@grep -h '^#\s*ifdef\sENABLE_' "$(ROOTDIR)/qmk_firmware/quantum/rgb_matrix"/*.h "$(ROOTDIR)/qmk_firmware/quantum/rgb_matrix"/*/*.h | sed -e 's@#\s*ifdef\s*@#    define @g' | sort | uniq >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo '#endif // RGB_MATRIX_ENABLE' >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo '#ifdef RGBLIGHT_ENABLE' >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@grep -h '^#\s*ifdef\s*RGBLIGHT_EFFECT' "$(ROOTDIR)/qmk_firmware/quantum/rgblight"/*.h | sed -e 's@#\s*ifdef\s*@#    define @g' | sort | uniq >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"
	@echo '#endif // RGBLIGHT_ENABLE' >> "$(ROOTDIR)/users-tzarc/enable_all_rgb_effects.h"

define handle_link_entry
link_source_$1 := $$(word 1,$$(subst !, ,$1))
link_target_$1 := $$(word 2,$$(subst !, ,$1))
link_files_$1 := $$(shell find $$(ROOTDIR)/$$(link_source_$1) -type f \( -name '*.h' -or -name '*.c' \) -and -not -name '*conf.h' -and -not -name 'board.c' -and -not -name 'board.h' | sort)
link_files_all_$1 := $$(shell find $$(ROOTDIR)/$$(link_source_$1) -type f | sort)

DOCKER_VOLUME_LIST += -v $$(shell readlink -f "$$(link_source_$1)"):/qmk_firmware/$$(link_target_$1)

extra-links: link_$$(link_source_$1)
link_$$(link_source_$1): qmk_firmware
	@if [ ! -L "$(ROOTDIR)/qmk_firmware/$$(link_target_$1)" ] ; then \
		echo -e "\e[38;5;14mSymlinking: $$(link_source_$1) -> $$(link_target_$1)\e[0m" ; \
		ln -sf $(ROOTDIR)/$$(link_source_$1) $(ROOTDIR)/qmk_firmware/$$(link_target_$1) ; \
		if [ -z "$$(grep -P '^$$(link_target_$1)$$$$' $(ROOTDIR)/qmk_firmware/.git/info/exclude 2>/dev/null)" ] ; then \
			echo $$(link_target_$1) >> $(ROOTDIR)/qmk_firmware/.git/info/exclude ; \
		fi ; \
	fi

clean: unlink_$$(link_source_$1)
distclean: unlink_$$(link_source_$1)
unlinks: unlink_$$(link_source_$1)
unlink_$$(link_source_$1): qmk_firmware
	@if [ -L "$(ROOTDIR)/qmk_firmware/$$(link_target_$1)" ] ; then \
		echo -e "\e[38;5;14mRemoving symlink: $$(link_source_$1) -> $$(link_target_$1)\e[0m" ; \
		rm $(ROOTDIR)/qmk_firmware/$$(link_target_$1) || true; \
		sed -i "\@^$$(link_target_$1)@d" $(ROOTDIR)/qmk_firmware/.git/info/exclude; \
	fi
endef

$(foreach link_entry,$(EXTRA_LINK_DEFS),$(eval $(call handle_link_entry,$(link_entry))))

define handle_board_entry
board_name_$1 := $$(word 1,$$(subst !, ,$1))
board_source_$1 := $$(word 2,$$(subst !, ,$1))
board_target_$1 := $$(word 3,$$(subst !, ,$1))
board_keymap_$1 := $$(word 4,$$(subst !, ,$1))
board_keyboard_$1 := $$(word 5,$$(subst !, ,$1))

ifeq ($$(board_keyboard_$1),)
board_keyboard_$1 := $$(board_target_$1)
endif

board_qmk_$1 := $$(shell echo $$(board_keyboard_$1) | sed -e 's@/keymaps/.*@@g')
board_file_$1 := $$(shell echo $$(board_qmk_$1) | sed -e 's@/@_@g' -e 's@:@_@g')
board_files_$1 := $$(shell find $$(ROOTDIR)/$$(board_source_$1) -type f \( -name '*.h' -or -name '*.c' \) -and -not -name '*conf.h' -and -not -name 'board.c' -and -not -name 'board.h' | sort)
board_files_all_$1 := $$(shell find $$(ROOTDIR)/$$(board_source_$1) -type f | sort)

bin_$$(board_name_$1): board_link_$$(board_name_$1) rgb_effects
	@echo -e "\e[38;5;14mBuilding: $$(board_qmk_$1):$$(board_keymap_$1)\e[0m"
	+@cd "$(ROOTDIR)/qmk_firmware" \
		&& { [ -z "$$(NO_COMPILEDB)" ] && qmk generate-compilation-database -kb $$(board_qmk_$1) -km $$(board_keymap_$1) || true; } \
		&& { \
			if [ -e "$(ROOTDIR)/qmk_firmware/builddefs/build_keyboard.mk" ] ; then \
				$$(MAKE) --no-print-directory -r -R -C "$(ROOTDIR)/qmk_firmware" -f "$(ROOTDIR)/qmk_firmware/builddefs/build_keyboard.mk" $$(MAKEFLAGS) KEYBOARD="$$(board_qmk_$1)" KEYMAP="$$(board_keymap_$1)" REQUIRE_PLATFORM_KEY= COLOR=true SILENT=false ; \
			else \
				$$(MAKE) --no-print-directory -r -R -C "$(ROOTDIR)/qmk_firmware" -f "$(ROOTDIR)/qmk_firmware/build_keyboard.mk" $$(MAKEFLAGS) KEYBOARD="$$(board_qmk_$1)" KEYMAP="$$(board_keymap_$1)" REQUIRE_PLATFORM_KEY= COLOR=true SILENT=false ; \
			fi ; \
		}
	@cp $$(ROOTDIR)/qmk_firmware/$$(board_file_$1)* $$(ROOTDIR) \
		|| true
	@[ -e "$$(ROOTDIR)/qmk_firmware/compile_commands.json" ] && cp $$(ROOTDIR)/qmk_firmware/compile_commands.json $$(ROOTDIR) \
		|| true

tidy_$$(board_name_$1): bin_$$(board_name_$1)
	@echo -e "\e[38;5;14mRunning clang-tidy on: $$(board_qmk_$1):$$(board_keymap_$1)\e[0m"
	@rm -f "$$(ROOTDIR)/clang-tidy_$$(board_name_$1).log" || true
	cd "$(ROOTDIR)/qmk_firmware" \
		&& $(CLANG_TIDY) -p . keyboards drivers quantum tmk_core -j9 -checks '$(CLANG_TIDY_CHECKS)' > "$$(ROOTDIR)/clang-tidy_$$(board_name_$1).log" 2>&1 \
		|| true

db_$$(board_name_$1): board_link_$$(board_name_$1)
	@echo -e "\e[38;5;14mCreating compiledb for: $$(board_qmk_$1):$$(board_keymap_$1)\e[0m"
	cd "$(ROOTDIR)/qmk_firmware" \
		&& $$(MAKE) distclean \
		&& qmk generate-compilation-database -kb $$(board_qmk_$1) -km $$(board_keymap_$1)
	@cp $$(ROOTDIR)/qmk_firmware/compile_commands.json $$(ROOTDIR) || true

flash_$$(board_name_$1): bin_$$(board_name_$1)
	@echo -e "\e[38;5;14mFlashing: $$(board_qmk_$1):$$(board_keymap_$1)\e[0m"
	cd "$(ROOTDIR)/qmk_firmware" \
		&& qmk flash -kb $$(board_qmk_$1) -km $$(board_keymap_$1)

$$(board_name_$1): bin_$$(board_name_$1)
bin: bin_$$(board_name_$1)

board_link_$$(board_name_$1): extra-links
	@if [ ! -L "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" ] ; then \
		echo -e "\e[38;5;14mSymlinking: $$(board_source_$1) -> $$(board_target_$1)\e[0m" ; \
		if [ ! -d "$$(shell dirname "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)")" ] ; then \
			mkdir -p "$$(shell dirname "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)")" ; \
		fi ; \
		ln -sf "$$(ROOTDIR)/$$(board_source_$1)" "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" ; \
		if [ -z "$$(grep -P '^keyboards/$$(board_target_$1)$$$$' $(ROOTDIR)/qmk_firmware/.git/info/exclude 2>/dev/null)" ] ; then \
			echo keyboards/$$(board_target_$1) >> $(ROOTDIR)/qmk_firmware/.git/info/exclude ; \
		fi ; \
	fi
	@touch $$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)

DOCKER_VOLUME_LIST += -v $$(shell readlink -f "$$(board_source_$1)"):/qmk_firmware/keyboards/$$(board_target_$1)

board_unlink_$$(board_name_$1):
	@if [ -L "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" ] ; then \
		echo -e "\e[38;5;14mRemoving symlink: $$(board_target_$1)\e[0m" ; \
		rm "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" || true; \
		sed -i "\@^keyboards/$$(board_target_$1)@d" $(ROOTDIR)/qmk_firmware/.git/info/exclude; \
	fi

links: board_link_$$(board_name_$1)
unlinks: board_unlink_$$(board_name_$1)
clean: board_unlink_$$(board_name_$1)
distclean: board_unlink_$$(board_name_$1)
endef

$(foreach board_entry,$(BOARD_DEFS),$(eval $(call handle_board_entry,$(board_entry))))

#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#CONTAINER_PREAMBLE := export HOME="/tmp"; export PYTHONUSERBASE="/tmp/python"; export PATH="\$$PYTHONUSERBASE/bin:\$$PATH"; python3 -m pip install --upgrade pip; python3 -m pip install -r requirements-dev.txt
CONTAINER_PREAMBLE := export HOME="/tmp"; export PATH="/tmp/.local/bin:\$$PATH"; python3 -m pip install --upgrade pip; python3 -m pip install -r requirements-dev.txt

format-core:
	cd $(ROOTDIR)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk format-c --core-only -a" \
		&& qmk format-python -a

pytest:
	cd $(ROOTDIR)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); qmk pytest"

container-shell:
	cd $(ROOTDIR)/qmk_firmware \
		&& RUNTIME=docker ./util/docker_cmd.sh bash -lic "$(CONTAINER_PREAMBLE); exec bash"

docker-test:
	@docker run --rm \
		--user $(shell id -u):$(shell id -g) \
		-w /qmk_firmware \
		-v $(shell readlink -f "$(ROOTDIR)/qmk_firmware"):/qmk_firmware \
		$(DOCKER_VOLUME_LIST) \
		qmkfm/qmk_cli:latest \
		qmk compile -j 20 -kb annepro2/c18 -km tzarc
