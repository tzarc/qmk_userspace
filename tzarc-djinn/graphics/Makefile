################################################################################################################################################################
# Generated files

QMK_BIN := qmk
USER_PATH := .
GENERATED_OUTPUT := ./src

all: generated-files

clean:
	rm -rf "$(GENERATED_OUTPUT)"

.INTERMEDIATE: srcdir

srcdir:
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"

distclean: clean

################################################################################################################################################################
# Fonts

generated-files: $(GENERATED_OUTPUT)/thintel15.h

$(GENERATED_OUTPUT)/thintel15.png: srcdir $(USER_PATH)/Makefile $(USER_PATH)/Thintel.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Thintel.ttf --size 15 -o $(GENERATED_OUTPUT)/thintel15.png

$(GENERATED_OUTPUT)/thintel15.h: srcdir $(GENERATED_OUTPUT)/thintel15.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/thintel15.png -f mono2

################################################################################################################################################################
# Djinn logo

generated-files: $(GENERATED_OUTPUT)/djinn.h

$(GENERATED_OUTPUT)/djinn.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/djinn.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/djinn.png -o $(GENERATED_OUTPUT) -f mono4 --rle

################################################################################################################################################################
# Key lock images

LOCK_IMAGE_FORMAT := mono4

generated-files: $(GENERATED_OUTPUT)/lock-caps-ON.h $(GENERATED_OUTPUT)/lock-caps-OFF.h

$(GENERATED_OUTPUT)/lock-caps-ON.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-caps-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-caps-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT) --rle

$(GENERATED_OUTPUT)/lock-caps-OFF.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-caps-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-caps-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT) --rle

generated-files: $(GENERATED_OUTPUT)/lock-num-ON.h $(GENERATED_OUTPUT)/lock-num-OFF.h

$(GENERATED_OUTPUT)/lock-num-ON.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-num-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-num-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT) --rle

$(GENERATED_OUTPUT)/lock-num-OFF.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-num-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-num-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT) --rle

generated-files: $(GENERATED_OUTPUT)/lock-scrl-ON.h $(GENERATED_OUTPUT)/lock-scrl-OFF.h

$(GENERATED_OUTPUT)/lock-scrl-ON.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-scrl-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-scrl-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT) --rle

$(GENERATED_OUTPUT)/lock-scrl-OFF.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-scrl-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-scrl-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT) --rle
