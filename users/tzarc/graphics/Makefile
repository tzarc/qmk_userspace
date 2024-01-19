# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
################################################################################################################################################################
# Generated files

QMK_BIN := qmk
USER_PATH := .
GENERATED_OUTPUT := ./src

all: generated-files

clean:
	rm -rf "$(GENERATED_OUTPUT)"

output_dir:
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"

distclean: clean

################################################################################################################################################################
# Game images

generated-files: $(GENERATED_OUTPUT)/diablo.qgf.h $(GENERATED_OUTPUT)/wow.qgf.h

$(GENERATED_OUTPUT)/diablo.qgf.h: output_dir $(USER_PATH)/Makefile $(USER_PATH)/diablo.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/diablo.png -o $(GENERATED_OUTPUT) -f pal16

$(GENERATED_OUTPUT)/wow.qgf.h: output_dir $(USER_PATH)/Makefile $(USER_PATH)/wow.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/wow.png -o $(GENERATED_OUTPUT) -f pal16

################################################################################################################################################################
# Game fonts

generated-files: $(GENERATED_OUTPUT)/exocet.qff.h

$(GENERATED_OUTPUT)/exocet.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/exocetlight-medium.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/exocetlight-medium.ttf --size 32 -o $(GENERATED_OUTPUT)/exocet.png --no-ascii --unicode-glyphs "1234Q"

$(GENERATED_OUTPUT)/exocet.qff.h: $(GENERATED_OUTPUT)/exocet.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/exocet.png -f mono4 --no-ascii --unicode-glyphs "1234Q"

################################################################################################################################################################
# Fixed-width fonts

generated-files: $(GENERATED_OUTPUT)/proggytiny.qff.h

$(GENERATED_OUTPUT)/proggytiny.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/ProggyTiny.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/ProggyTiny.ttf --size 13 -o $(GENERATED_OUTPUT)/proggytiny.png

$(GENERATED_OUTPUT)/proggytiny.qff.h: $(GENERATED_OUTPUT)/proggytiny.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/proggytiny.png -f mono4

generated-files: $(GENERATED_OUTPUT)/everex_5x8.qff.h

$(GENERATED_OUTPUT)/everex_5x8.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/Px437_EverexME_5x8.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Px437_EverexME_5x8.ttf --size 8 -o $(GENERATED_OUTPUT)/everex_5x8.png

$(GENERATED_OUTPUT)/everex_5x8.qff.h: $(GENERATED_OUTPUT)/everex_5x8.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/everex_5x8.png -f mono4

################################################################################################################################################################
# Fonts

generated-files: $(GENERATED_OUTPUT)/balthasar.qff.h

$(GENERATED_OUTPUT)/balthasar.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/BalthasarRegularNbp-va6O.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/BalthasarRegularNbp-va6O.ttf --size 16 -o $(GENERATED_OUTPUT)/balthasar.png

$(GENERATED_OUTPUT)/balthasar.qff.h: $(GENERATED_OUTPUT)/balthasar.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/balthasar.png -f mono2

generated-files: $(GENERATED_OUTPUT)/epilepsysansbold.qff.h

$(GENERATED_OUTPUT)/epilepsysansbold.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/EpilepsySansBold-o0W4.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/EpilepsySansBold-o0W4.ttf --size 13 -o $(GENERATED_OUTPUT)/epilepsysansbold.png

$(GENERATED_OUTPUT)/epilepsysansbold.qff.h: $(GENERATED_OUTPUT)/epilepsysansbold.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/epilepsysansbold.png -f mono4

generated-files: $(GENERATED_OUTPUT)/noto11.qff.h

$(GENERATED_OUTPUT)/noto11.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 11 -o $(GENERATED_OUTPUT)/noto11.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto11.qff.h: $(GENERATED_OUTPUT)/noto11.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto11.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/noto16.qff.h

$(GENERATED_OUTPUT)/noto16.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 16 -o $(GENERATED_OUTPUT)/noto16.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto16.qff.h: $(GENERATED_OUTPUT)/noto16.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto16.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/noto28.qff.h

$(GENERATED_OUTPUT)/noto28.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 28 -o $(GENERATED_OUTPUT)/noto28.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto28.qff.h: $(GENERATED_OUTPUT)/noto28.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto28.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/redalert13.qff.h

$(GENERATED_OUTPUT)/redalert13.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/CandC-Red-Alert-LAN.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/CandC-Red-Alert-LAN.ttf --size 13 -o $(GENERATED_OUTPUT)/redalert13.png

$(GENERATED_OUTPUT)/redalert13.qff.h: $(GENERATED_OUTPUT)/redalert13.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/redalert13.png -f mono2

generated-files: $(GENERATED_OUTPUT)/minecraftia8.qff.h

$(GENERATED_OUTPUT)/minecraftia8.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/Minecraftia-Regular.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Minecraftia-Regular.ttf --size 8 -o $(GENERATED_OUTPUT)/minecraftia8.png

$(GENERATED_OUTPUT)/minecraftia8.qff.h: $(GENERATED_OUTPUT)/minecraftia8.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/minecraftia8.png -f mono2

generated-files: $(GENERATED_OUTPUT)/thintel15.qff.h

$(GENERATED_OUTPUT)/thintel15.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/Thintel.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Thintel.ttf --size 15 -o $(GENERATED_OUTPUT)/thintel15.png

$(GENERATED_OUTPUT)/thintel15.qff.h: $(GENERATED_OUTPUT)/thintel15.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/thintel15.png -f mono2

generated-files: $(GENERATED_OUTPUT)/hannover23.qff.h

$(GENERATED_OUTPUT)/hannover23.png: output_dir $(USER_PATH)/Makefile $(USER_PATH)/HannoverMesseSans-dewK.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/HannoverMesseSans-dewK.ttf --size 23 -o $(GENERATED_OUTPUT)/hannover23.png

$(GENERATED_OUTPUT)/hannover23.qff.h: $(GENERATED_OUTPUT)/hannover23.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/hannover23.png -f mono2

################################################################################################################################################################
# Key lock images

LOCK_IMAGE_FORMAT := mono4
DJINN_GRAPHICS_PATH := ../../keyboards/tzarc-djinn-OLD/graphics

generated-files: $(GENERATED_OUTPUT)/lock-caps-ON.qgf.h $(GENERATED_OUTPUT)/lock-caps-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-caps-ON.qgf.h: output_dir $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-caps-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-caps-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-caps-OFF.qgf.h: output_dir $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-caps-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-caps-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-num-ON.qgf.h $(GENERATED_OUTPUT)/lock-num-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-num-ON.qgf.h: output_dir $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-num-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-num-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-num-OFF.qgf.h: output_dir $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-num-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-num-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-scrl-ON.qgf.h $(GENERATED_OUTPUT)/lock-scrl-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-scrl-ON.qgf.h: output_dir $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-scrl-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-scrl-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-scrl-OFF.qgf.h: output_dir $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-scrl-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-scrl-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)
