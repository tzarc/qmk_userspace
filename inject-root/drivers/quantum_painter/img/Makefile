CONVERT_GFX := ../../../util/convert_gfx.py

all: files

# QMK Logo generation
LOGO_FILES := dark light
LOGO_WIDTHS := 120 160 240

define handle_logo_width
logo_type_$1_$2 := $1
logo_width_$1_$2 := $2
logo_file_$1_$2 := badge-$$(logo_type_$1_$2)$$(logo_width_$1_$2)
logo_target_$1_$2 := $$(logo_file_$1_$2).h
$$(logo_target_$1_$2): Makefile $(CONVERT_GFX)
	@convert badge-$$(logo_type_$1_$2).svg -depth 8 -background black -geometry $$(logo_width_$1_$2)x $$(logo_file_$1_$2).png
	@$(CONVERT_GFX) -4 -i $$(logo_file_$1_$2).png -o $$(logo_file_$1_$2)
files: $$(logo_target_$1_$2)
endef

define handle_logo_file
$(foreach logo_width,$(LOGO_WIDTHS),$(eval $(call handle_logo_width,$(1),$(logo_width))))
endef

$(foreach logo_file,$(LOGO_FILES),$(eval $(call handle_logo_file,$(logo_file))))

qp_luts.h: Makefile $(CONVERT_GFX)
	@$(CONVERT_GFX) -r -l -o qp_luts

files: qp_luts.h

clean: Makefile
	rm gfx-* *.png qp_luts* || true