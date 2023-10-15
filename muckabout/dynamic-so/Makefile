TARGET_LIB ?= dynamic_lib.so

BUILD_DIR ?= .build
SRC_DIRS ?= src

SRCS := $(shell find $(SRC_DIRS) \( -name *.cpp -or -name *.cxx -or -name *.cc -or -name *.c -or -name *.s \))
HDRS := $(shell find $(SRC_DIRS) \( -name *.hpp -or -name *.hxx -or -name *.hh -or -name *.h \))
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)

QMK_HOME := $(shell qmk config user.qmk_home | cut -d= -f2)
INC_DIRS += $(QMK_HOME)/quantum

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
STRIP = arm-none-eabi-strip
READELF = arm-none-eabi-readelf
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

# STM32F405RG
QMK_COMPILE_FLAGS := -mcpu=cortex-m4 -mthumb -mno-thumb-interwork -mno-unaligned-access -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant \
	-mword-relocations -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-builtin -fno-common -fshort-wchar -ggdb -flto -Os \
	-Wall -Wstrict-prototypes -Werror -fcommon -fstack-usage #-mno-pic-data-is-text-relative #-msingle-pic-base

OPT_FLAGS = -g -O3
CFLAGS_BASE ?= $(OPT_FLAGS) -fPIE -fPIC -fpic -fpie

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
LDFLAGS ?= $(OPT_FLAGS) \
	-Bsymbolic -shared -nostdlib -nostartfiles -nodefaultlibs -mlong-calls -fno-common \
	-Wl,-Map=${TARGET_LIB}.map -Wl,--print-memory-usage -Wl,--gc-sections \
	--specs=nosys.specs -nostdlib -Wl,--undefined=main -Wl,--emit-relocs \
	$(QMK_COMPILE_FLAGS) \
	-T src/lib.ld
CFLAGS ?= -std=c11 $(CFLAGS_BASE) $(LDFLAGS) $(QMK_COMPILE_FLAGS)
CXXFLAGS ?= -std=c++14 -fno-exceptions -fno-rtti $(CFLAGS_BASE) $(LDFLAGS) $(QMK_COMPILE_FLAGS)

###########

all: $(TARGET_LIB)

.PHONY: buildreqs
buildreqs: compile_commands.json Makefile

format:
	black -l 160 convert-binary.py

convert: $(TARGET_LIB)
	if false ; then $(STRIP) -s $(TARGET_LIB) ; fi
	if true ; then $(OBJCOPY) --strip-debug --strip-dwo $(TARGET_LIB) ; fi
	if true ; then ./convert-binary.py $(TARGET_LIB) ; fi

info: convert
	$(OBJDUMP) -afphs $(TARGET_LIB) > $(TARGET_LIB).dump
	$(OBJDUMP) -DSz $(TARGET_LIB) > $(TARGET_LIB).asm
	$(READELF) -sSW ${TARGET_LIB} > $(TARGET_LIB).symbols
	$(READELF) -r ${TARGET_LIB} > $(TARGET_LIB).relocs
	$(READELF) -l ${TARGET_LIB}
	$(SIZE) $(TARGET_LIB)
	file ${TARGET_LIB}
	stat ${TARGET_LIB}
	stat ${TARGET_LIB}.qke

compile_commands.json: Makefile
	@compiledb make -n all >/dev/null

define setup_formatter
format: format_$1
format_$1: $1
	clang-format -i "$1"
endef
$(foreach src,$(SRCS) $(HDRS),$(eval $(call setup_formatter,$(src))))

$(BUILD_DIR)/$(TARGET_LIB): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s buildreqs
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c sources
$(BUILD_DIR)/%.c.o: %.c buildreqs
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ sources
$(BUILD_DIR)/%.cpp.o: %.cpp buildreqs
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cxx.o: %.cxx buildreqs
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc buildreqs
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) .cache compile_commands.json $(TARGET_LIB) $(TARGET_LIB).asm $(TARGET_LIB).dump $(TARGET_LIB).map $(TARGET_LIB).relocs $(TARGET_LIB).symbols $(TARGET_LIB).qke

.PHONY: distclean

distclean: clean

$(TARGET_LIB): $(BUILD_DIR)/$(TARGET_LIB)
	cp -f $(BUILD_DIR)/$(TARGET_LIB) $(TARGET_LIB)

-include $(DEPS)

MKDIR_P ?= @mkdir -p