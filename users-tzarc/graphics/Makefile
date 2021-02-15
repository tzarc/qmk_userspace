################################################################################################################################################################
# Generated files

QMK_BIN := ../../qmk_firmware/bin/qmk
USER_PATH := .
GENERATED_OUTPUT := ./src

all: generated-files

clean:
	rm -rf "$(GENERATED_OUTPUT)"

distclean: clean

################################################################################################################################################################
# Fonts

generated-files: $(GENERATED_OUTPUT)/noto16.h

$(GENERATED_OUTPUT)/noto16.png: $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 16 -o $(GENERATED_OUTPUT)/noto16.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto16.h: $(GENERATED_OUTPUT)/noto16.png
	[ -d $(GENERATED_OUTPUT) ] || mkdir -p $(GENERATED_OUTPUT)
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto16.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/noto28.h

$(GENERATED_OUTPUT)/noto28.png: $(USER_PATH)/Makefile $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/NotoSans-ExtraCondensedBold.ttf --size 28 -o $(GENERATED_OUTPUT)/noto28.png --unicode-glyphs "ĄȽɂɻɣɈʣ"

$(GENERATED_OUTPUT)/noto28.h: $(GENERATED_OUTPUT)/noto28.png
	[ -d $(GENERATED_OUTPUT) ] || mkdir -p $(GENERATED_OUTPUT)
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/noto28.png -f mono4 --unicode-glyphs "ĄȽɂɻɣɈʣ"

generated-files: $(GENERATED_OUTPUT)/redalert13.h

$(GENERATED_OUTPUT)/redalert13.png: $(USER_PATH)/Makefile $(USER_PATH)/CandC-Red-Alert-LAN.ttf
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/CandC-Red-Alert-LAN.ttf --size 13 -o $(GENERATED_OUTPUT)/redalert13.png

$(GENERATED_OUTPUT)/redalert13.h: $(GENERATED_OUTPUT)/redalert13.png
	[ -d $(GENERATED_OUTPUT) ] || mkdir -p $(GENERATED_OUTPUT)
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/redalert13.png -f mono2

################################################################################################################################################################
# Key lock images

LOCK_IMAGE_FORMAT := mono4

generated-files: $(GENERATED_OUTPUT)/lock-caps-ON.h $(GENERATED_OUTPUT)/lock-caps-OFF.h

$(GENERATED_OUTPUT)/lock-caps-ON.h: $(USER_PATH)/Makefile $(USER_PATH)/lock-caps-ON.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-caps-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-caps-OFF.h: $(USER_PATH)/Makefile $(USER_PATH)/lock-caps-OFF.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-caps-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-num-ON.h $(GENERATED_OUTPUT)/lock-num-OFF.h

$(GENERATED_OUTPUT)/lock-num-ON.h: $(USER_PATH)/Makefile $(USER_PATH)/lock-num-ON.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-num-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-num-OFF.h: $(USER_PATH)/Makefile $(USER_PATH)/lock-num-OFF.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-num-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-scrl-ON.h $(GENERATED_OUTPUT)/lock-scrl-OFF.h

$(GENERATED_OUTPUT)/lock-scrl-ON.h: $(USER_PATH)/Makefile $(USER_PATH)/lock-scrl-ON.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-scrl-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-scrl-OFF.h: $(USER_PATH)/Makefile $(USER_PATH)/lock-scrl-OFF.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-scrl-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)