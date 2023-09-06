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
STRIP_S = arm-none-eabi-strip -s
READELF = arm-none-eabi-readelf

OPT_FLAGS = -g -O3
CFLAGS_BASE ?= $(OPT_FLAGS) -fpic -fpie

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
LDFLAGS ?= $(OPT_FLAGS) -shared -nostdlib -nostartfiles -nodefaultlibs
CFLAGS ?= -std=c11 $(CFLAGS_BASE) $(LDFLAGS)
CXXFLAGS ?= -std=c++14 $(CFLAGS_BASE) $(LDFLAGS)

###########

all: $(TARGET_LIB)

strip: $(TARGET_LIB)
	$(STRIP_S) $(TARGET_LIB)

info: strip
	file ${TARGET_LIB}
	$(READELF) -sSW ${TARGET_LIB}

compile_commands.json:
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
$(BUILD_DIR)/%.s.o: %.s compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c sources
$(BUILD_DIR)/%.c.o: %.c compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ sources
$(BUILD_DIR)/%.cpp.o: %.cpp compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cxx.o: %.cxx compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) compile_commands.json

.PHONY: distclean

distclean: clean

$(TARGET_LIB): $(BUILD_DIR)/$(TARGET_LIB)
	cp -f $(BUILD_DIR)/$(TARGET_LIB) $(TARGET_LIB)

-include $(DEPS)

MKDIR_P ?= @mkdir -p