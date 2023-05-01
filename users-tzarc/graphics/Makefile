# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
################################################################################################################################################################
# Generated files

QMK_BIN := qmk
USER_PATH := .
GENERATED_OUTPUT := ./src

all: generated-files

clean:
	rm -rf "$(GENERATED_OUTPUT)"

distclean: clean

################################################################################################################################################################
# Game images

generated-files: $(GENERATED_OUTPUT)/diablo3.qgf.h $(GENERATED_OUTPUT)/wow.qgf.h

$(GENERATED_OUTPUT)/diablo3.qgf.h: $(USER_PATH)/Makefile $(USER_PATH)/diablo3.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/diablo3.png -o $(GENERATED_OUTPUT) -f rgb565

$(GENERATED_OUTPUT)/wow.qgf.h: $(USER_PATH)/Makefile $(USER_PATH)/wow.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/wow.png -o $(GENERATED_OUTPUT) -f rgb565

################################################################################################################################################################
# Game fonts

generated-files: $(GENERATED_OUTPUT)/avqest.qff.h

$(GENERATED_OUTPUT)/avqest.png: $(USER_PATH)/Makefile $(USER_PATH)/AvQest.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/AvQest.ttf --size 40 -o $(GENERATED_OUTPUT)/avqest.png --no-ascii --unicode-glyphs "1234"

$(GENERATED_OUTPUT)/avqest.qff.h: $(GENERATED_OUTPUT)/avqest.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/avqest.png -f mono4 --no-ascii --unicode-glyphs "1234"

################################################################################################################################################################
# Fixed-width fonts

generated-files: $(GENERATED_OUTPUT)/proggytiny.qff.h

$(GENERATED_OUTPUT)/proggytiny.png: $(USER_PATH)/Makefile $(USER_PATH)/ProggyTiny.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/ProggyTiny.ttf --size 13 -o $(GENERATED_OUTPUT)/proggytiny.png

$(GENERATED_OUTPUT)/proggytiny.qff.h: $(GENERATED_OUTPUT)/proggytiny.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/proggytiny.png -f mono4

generated-files: $(GENERATED_OUTPUT)/everex_5x8.qff.h

$(GENERATED_OUTPUT)/everex_5x8.png: $(USER_PATH)/Makefile $(USER_PATH)/Px437_EverexME_5x8.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Px437_EverexME_5x8.ttf --size 8 -o $(GENERATED_OUTPUT)/everex_5x8.png

$(GENERATED_OUTPUT)/everex_5x8.qff.h: $(GENERATED_OUTPUT)/everex_5x8.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/everex_5x8.png -f mono4

################################################################################################################################################################
# Fonts

generated-files: $(GENERATED_OUTPUT)/balthasar.qff.h

$(GENERATED_OUTPUT)/balthasar.png: $(USER_PATH)/Makefile $(USER_PATH)/BalthasarRegularNbp-va6O.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/BalthasarRegularNbp-va6O.ttf --size 16 -o $(GENERATED_OUTPUT)/balthasar.png

$(GENERATED_OUTPUT)/balthasar.qff.h: $(GENERATED_OUTPUT)/balthasar.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/balthasar.png -f mono2

generated-files: $(GENERATED_OUTPUT)/epilepsysansbold.qff.h

$(GENERATED_OUTPUT)/epilepsysansbold.png: $(USER_PATH)/Makefile $(USER_PATH)/EpilepsySansBold-o0W4.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/EpilepsySansBold-o0W4.ttf --size 13 -o $(GENERATED_OUTPUT)/epilepsysansbold.png

$(GENERATED_OUTPUT)/epilepsysansbold.qff.h: $(GENERATED_OUTPUT)/epilepsysansbold.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/epilepsysansbold.png -f mono4

generated-files: $(GENERATED_OUTPUT)/noto11.qff.h

$(GENERATED_OUTPUT)/noto11.png: $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 11 -o $(GENERATED_OUTPUT)/noto11.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto11.qff.h: $(GENERATED_OUTPUT)/noto11.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto11.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/noto16.qff.h

$(GENERATED_OUTPUT)/noto16.png: $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 16 -o $(GENERATED_OUTPUT)/noto16.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto16.qff.h: $(GENERATED_OUTPUT)/noto16.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto16.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/noto28.qff.h

$(GENERATED_OUTPUT)/noto28.png: $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 28 -o $(GENERATED_OUTPUT)/noto28.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto28.qff.h: $(GENERATED_OUTPUT)/noto28.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto28.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/redalert13.qff.h

$(GENERATED_OUTPUT)/redalert13.png: $(USER_PATH)/Makefile $(USER_PATH)/CandC-Red-Alert-LAN.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/CandC-Red-Alert-LAN.ttf --size 13 -o $(GENERATED_OUTPUT)/redalert13.png

$(GENERATED_OUTPUT)/redalert13.qff.h: $(GENERATED_OUTPUT)/redalert13.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/redalert13.png -f mono2

generated-files: $(GENERATED_OUTPUT)/minecraftia8.qff.h

$(GENERATED_OUTPUT)/minecraftia8.png: $(USER_PATH)/Makefile $(USER_PATH)/Minecraftia-Regular.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Minecraftia-Regular.ttf --size 8 -o $(GENERATED_OUTPUT)/minecraftia8.png

$(GENERATED_OUTPUT)/minecraftia8.qff.h: $(GENERATED_OUTPUT)/minecraftia8.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/minecraftia8.png -f mono2

generated-files: $(GENERATED_OUTPUT)/thintel15.qff.h

$(GENERATED_OUTPUT)/thintel15.png: $(USER_PATH)/Makefile $(USER_PATH)/Thintel.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Thintel.ttf --size 15 -o $(GENERATED_OUTPUT)/thintel15.png

$(GENERATED_OUTPUT)/thintel15.qff.h: $(GENERATED_OUTPUT)/thintel15.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/thintel15.png -f mono2

generated-files: $(GENERATED_OUTPUT)/hannover23.qff.h

$(GENERATED_OUTPUT)/hannover23.png: $(USER_PATH)/Makefile $(USER_PATH)/HannoverMesseSans-dewK.ttf
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/HannoverMesseSans-dewK.ttf --size 23 -o $(GENERATED_OUTPUT)/hannover23.png

$(GENERATED_OUTPUT)/hannover23.qff.h: $(GENERATED_OUTPUT)/hannover23.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/hannover23.png -f mono2

################################################################################################################################################################
# Key lock images

LOCK_IMAGE_FORMAT := mono4
DJINN_GRAPHICS_PATH := ../../keyboards/tzarc-djinn-OLD/graphics

generated-files: $(GENERATED_OUTPUT)/lock-caps-ON.qgf.h $(GENERATED_OUTPUT)/lock-caps-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-caps-ON.qgf.h: $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-caps-ON.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-caps-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-caps-OFF.qgf.h: $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-caps-OFF.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-caps-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-num-ON.qgf.h $(GENERATED_OUTPUT)/lock-num-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-num-ON.qgf.h: $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-num-ON.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-num-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-num-OFF.qgf.h: $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-num-OFF.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-num-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-scrl-ON.qgf.h $(GENERATED_OUTPUT)/lock-scrl-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-scrl-ON.qgf.h: $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-scrl-ON.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-scrl-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-scrl-OFF.qgf.h: $(USER_PATH)/Makefile $(DJINN_GRAPHICS_PATH)/lock-scrl-OFF.png
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(DJINN_GRAPHICS_PATH)/lock-scrl-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)
