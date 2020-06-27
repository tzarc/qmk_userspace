CONVERT_GFX := ../../../util/convert_gfx.py

all: files

# QMK Logo generation
LOGO_BPP := 1 2 4
LOGO_FILES := dark light
LOGO_WIDTHS := 120 160 240

define handle_logo_bpp
logo_type_$1_$2_$3 := $1
logo_width_$1_$2_$3 := $2
logo_bpp_$1_$2_$3 := $3
logo_file_$1_$2_$3 := badge-$$(logo_type_$1_$2_$3)_$$(logo_width_$1_$2_$3)px_$$(logo_bpp_$1_$2_$3)bpp
logo_target_$1_$2_$3 := $$(logo_file_$1_$2_$3).h
$$(logo_target_$1_$2_$3): Makefile $(CONVERT_GFX)
	@convert badge-$$(logo_type_$1_$2_$3).svg -depth 8 -background black -geometry $$(logo_width_$1_$2_$3)x $$(logo_file_$1_$2_$3).png
	@$(CONVERT_GFX) -$$(logo_bpp_$1_$2_$3) -i $$(logo_file_$1_$2_$3).png -o $$(logo_file_$1_$2_$3)
files: $$(logo_target_$1_$2_$3)
endef

define handle_logo_width
$(foreach logo_bpp,$(LOGO_BPP),$(eval $(call handle_logo_bpp,$(1),$(2),$(logo_bpp))))
endef

define handle_logo_file
$(foreach logo_width,$(LOGO_WIDTHS),$(eval $(call handle_logo_width,$(1),$(logo_width))))
endef

$(foreach logo_file,$(LOGO_FILES),$(eval $(call handle_logo_file,$(logo_file))))

clean: Makefile
	rm gfx-* *.png || true