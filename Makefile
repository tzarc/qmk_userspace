export ROOTDIR := $(shell pwd)
#export PATH := /home/nickb/gcc-arm/gcc-arm-none-eabi-9-2019-q4-major/bin/:$(PATH)

BOARD_DEFS := \
	iris!tzarc-iris_rev4!keebio/iris/rev4/keymaps/tzarc!tzarc \
	ctrl!tzarc-ctrl!massdrop/ctrl/keymaps/tzarc!tzarc \
	luddite!tzarc-luddite!40percentclub/luddite/keymaps/tzarc!tzarc \
	mysterium-nick!tzarc-mysterium!coseyfannitutti/mysterium/keymaps/tzarc!tzarc \
	mysterium-dad!tzarc-mysterium-dad!coseyfannitutti/mysterium/keymaps/tzarc-dad!tzarc-dad \
	chocopad!tzarc-chocopad!keebio/chocopad/keymaps/tzarc!tzarc \
	cyclone!tzarc-cyclone!handwired/tzarc/cyclone!default \
	onekey_l152!alternates/nucleo64_l152re!handwired/onekey/nucleo64_l152re!default \
	onekey_g431!alternates/nucleo64_g431rb!handwired/onekey/nucleo64_g431rb!default \
	onekey_g474!alternates/nucleo64_g474re!handwired/onekey/nucleo64_g474re!default \

all: bin

remove_artifacts:
	rm "$(ROOTDIR)"/*.bin "$(ROOTDIR)"/*.hex "$(ROOTDIR)"/*.dump "$(ROOTDIR)"/.clang-format >/dev/null 2>&1 || true

clean: remove_artifacts
	@$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" clean 2>&1 \
		| egrep --line-buffered -iv '(Entering|Leaving) directory' \
		| egrep --line-buffered -iv 'Bad file descriptor' || true

distclean: remove_artifacts
	@$(MAKE) $(MAKEFLAGS) -C "$(ROOTDIR)/qmk_firmware" distclean 2>&1 \
		| egrep --line-buffered -iv '(Entering|Leaving) directory' \
		| egrep --line-buffered -iv 'Bad file descriptor' || true

format_prereq:
	@ln -sf $(ROOTDIR)/qmk_firmware/.clang-format $(ROOTDIR)/.clang-format

format: format_prereq

define handle_board_entry
board_name_$1 := $$(word 1,$$(subst !, ,$1))
board_source_$1 := $$(word 2,$$(subst !, ,$1))
board_target_$1 := $$(word 3,$$(subst !, ,$1))
board_keymap_$1 := $$(word 4,$$(subst !, ,$1))
board_qmk_$1 := $$(shell echo $$(board_target_$1) | sed -e 's@/keymaps/.*@@g')
board_file_$1 := $$(shell echo $$(board_qmk_$1) | sed -e 's@/@_@g' -e 's@:@_@g')
board_files_$1 := $$(shell find $$(ROOTDIR)/$$(board_source_$1) -type f \( -name '*.h' -or -name '*.c' \) -and -not -name '*conf.h' -and -not -name 'board.c' -and -not -name 'board.h' | sort)

bin_$$(board_name_$1): board_link_$$(board_name_$1)
	@echo "\e[38;5;14mBuilding: $$(board_qmk_$1)\e[0m"
	@$(MAKE) -C "$(ROOTDIR)/qmk_firmware" $$(board_qmk_$1):$$(board_keymap_$1) 2>&1 \
		| egrep --line-buffered -iv '(Entering|Leaving) directory' \
		| egrep --line-buffered -iv 'Bad file descriptor'
	@cp $$(ROOTDIR)/qmk_firmware/$$(board_file_$1)* $$(ROOTDIR)

flash_$$(board_name_$1): bin_$$(board_name_$1)
	@echo "\e[38;5;14mFlashing: $$(board_qmk_$1)\e[0m"
	@$(MAKE) -C "$(ROOTDIR)/qmk_firmware" $$(board_qmk_$1):$$(board_keymap_$1):flash

format_$$(board_name_$1): format_prereq
	@for file in $$(board_files_$1) ; do \
		echo "\e[38;5;14mFormatting: $$$$file\e[0m" ; \
		clang-format-7 -i "$$$$file" >/dev/null 2>&1 ; \
		dos2unix "$$$$file" >/dev/null 2>&1 ; \
	done

format: format_$$(board_name_$1)

$$(board_name_$1): bin_$$(board_name_$1)
bin: bin_$$(board_name_$1)

board_link_$$(board_name_$1):
	@if [ ! -L "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" ] ; then \
		echo "\e[38;5;14mLinking: $$(board_source_$1) -> $$(board_target_$1)\e[0m" ; \
		if [ ! -d "$$(shell dirname "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)")" ] ; then \
			mkdir -p "$$(shell dirname "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)")" ; \
		fi ; \
		ln -sf "$$(ROOTDIR)/$$(board_source_$1)" "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" ; \
	fi ; \
	touch $$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)

board_unlink_$$(board_name_$1):
	@if [ -L "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" ] ; then \
		echo "\e[38;5;14mRemoving link: $$(board_target_$1)\e[0m" ; \
		rm "$$(ROOTDIR)/qmk_firmware/keyboards/$$(board_target_$1)" ; \
	fi

links: board_link_$$(board_name_$1)
unlinks: board_unlink_$$(board_name_$1)
clean: board_unlink_$$(board_name_$1)
distclean: board_unlink_$$(board_name_$1)
endef

$(foreach board_entry,$(BOARD_DEFS),$(eval $(call handle_board_entry,$(board_entry))))