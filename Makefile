export ROOTDIR := $(shell pwd)
export PATH := $(HOME)/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$(PATH)

all: bin

update:
	git submodule update --remote
	make -C "$(ROOTDIR)/qmk_firmware" git-submodule

clean:
	rm *.bin || true
	make -C "$(ROOTDIR)/qmk_firmware" clean || true

distclean:
	rm *.bin || true
	make -C "$(ROOTDIR)/qmk_firmware" distclean || true

bin: tzarc_cyclone_L412_default.bin

CYCLONE_DEPS = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/tzarc/cyclone_L412" -type f)
tzarc_cyclone_L412_default.bin: $(CYCLONE_DEPS)
	#make -C "$(ROOTDIR)/qmk_firmware" tzarc/cyclone_L412:default
	make -j$(shell nproc) -C "$(ROOTDIR)/qmk_firmware" tzarc/cyclone_L412:default
	cp "$(ROOTDIR)/qmk_firmware/tzarc_cyclone_L412_default.bin" "$(ROOTDIR)"

flash: stlink bin
	st-flash write "$(ROOTDIR)/tzarc_cyclone_L412_default.bin" 0x08000000

debug: bin
	{ cd "$(ROOTDIR)/qmk_firmware" && arm-none-eabi-gdb --command="$(ROOTDIR)/gdbinit" --exec="$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L412_default.elf" --symbols="$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L412_default.elf" ; }

dump: bin
	arm-none-eabi-readelf -e "$(ROOTDIR)/qmk_firmware/.build/tzarc_cyclone_L412_default.elf"

CYCLONE_FORMATTABLE_FILES = $(shell find "$(ROOTDIR)/qmk_firmware/keyboards/tzarc/cyclone_L412" -maxdepth 1 -type f \( -name 'config.h' -or -name 'cyclone*.h' -or -name 'cyclone*.c' -or -name 'cyclone*.cpp' \))
format:
	@for file in $(CYCLONE_FORMATTABLE_FILES); do clang-format -i "$$file" ; dos2unix "$$file" ; done

stlink: /usr/local/bin/st-flash

/usr/local/bin/st-flash:
	sudo apt install -y build-essential cmake libusb-1.0 libusb-1.0-0-dev
	[ -d "$(ROOTDIR)/stlink-repo/.git" ] || git clone https://github.com/texane/stlink.git "$(ROOTDIR)/stlink-repo"
	[ -d "$(ROOTDIR)/stlink-build" ] || mkdir -p "$(ROOTDIR)/stlink-build"
	{ umask 022; cd "$(ROOTDIR)/stlink-repo" && patch -p1 < "$(ROOTDIR)/patches/stlink_L412.patch" ; }
	{ umask 022; cd "$(ROOTDIR)/stlink-build" && cmake -DCMAKE_BUILD_TYPE=Release ../stlink-repo && make -j$(shell nproc) && sudo make install && sudo ldconfig ; }
	rm -rf "$(ROOTDIR)/stlink-build" "$(ROOTDIR)/stlink-repo"
