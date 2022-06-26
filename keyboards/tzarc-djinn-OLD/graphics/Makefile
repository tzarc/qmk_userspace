################################################################################################################################################################
# Generated files

QMK_BIN := qmk
USER_PATH := .
GENERATED_OUTPUT := .

all: generated-files

clean:
	rm -rf "$(GENERATED_OUTPUT)"

.INTERMEDIATE: srcdir

srcdir:
	@[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"

generated-files: srcdir

distclean: clean

################################################################################################################################################################
# Fonts

generated-files: $(GENERATED_OUTPUT)/thintel15.qff.h

$(GENERATED_OUTPUT)/thintel15.png: srcdir $(USER_PATH)/Makefile $(USER_PATH)/Thintel.ttf
	$(QMK_BIN) painter-make-font-image --font $(USER_PATH)/Thintel.ttf --size 15 --output $(GENERATED_OUTPUT)/thintel15.png

$(GENERATED_OUTPUT)/thintel15.qff.h: srcdir $(GENERATED_OUTPUT)/thintel15.png
	$(QMK_BIN) painter-convert-font-image --input $(GENERATED_OUTPUT)/thintel15.png --format mono2

################################################################################################################################################################
# Djinn logo

generated-files: $(GENERATED_OUTPUT)/djinn.qgf.h

$(GENERATED_OUTPUT)/djinn.qgf.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/djinn.png
	[ -d "$(GENERATED_OUTPUT)" ] || mkdir -p "$(GENERATED_OUTPUT)"
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/djinn.png -o $(GENERATED_OUTPUT) -f mono4

################################################################################################################################################################
# Key lock images

LOCK_IMAGE_FORMAT := mono4

generated-files: $(GENERATED_OUTPUT)/lock-caps-ON.qgf.h $(GENERATED_OUTPUT)/lock-caps-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-caps-ON.qgf.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-caps-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-caps-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-caps-OFF.qgf.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-caps-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-caps-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-num-ON.qgf.h $(GENERATED_OUTPUT)/lock-num-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-num-ON.qgf.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-num-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-num-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-num-OFF.qgf.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-num-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-num-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

generated-files: $(GENERATED_OUTPUT)/lock-scrl-ON.qgf.h $(GENERATED_OUTPUT)/lock-scrl-OFF.qgf.h

$(GENERATED_OUTPUT)/lock-scrl-ON.qgf.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-scrl-ON.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-scrl-ON.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)

$(GENERATED_OUTPUT)/lock-scrl-OFF.qgf.h: srcdir $(USER_PATH)/Makefile $(USER_PATH)/lock-scrl-OFF.png
	$(QMK_BIN) painter-convert-graphics -i $(USER_PATH)/lock-scrl-OFF.png -o $(GENERATED_OUTPUT) -f $(LOCK_IMAGE_FORMAT)
