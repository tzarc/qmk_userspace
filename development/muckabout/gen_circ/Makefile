# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
TARGET_EXEC ?= gen_circ

BUILD_DIR ?= .build
SRC_DIRS ?= src

SRCS := $(shell find $(SRC_DIRS) \( -name *.cpp -or -name *.cxx -or -name *.cc -or -name *.c -or -name *.s \))
HDRS := $(shell find $(SRC_DIRS) \( -name *.hpp -or -name *.hxx -or -name *.hh -or -name *.h \))
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC = gcc
CXX = g++
STRIP = strip
READELF = readelf
OBJCOPY = objcopy
OBJDUMP = objdump
SIZE = size

OPT_FLAGS = -g -O0
CFLAGS_BASE ?= $(OPT_FLAGS)

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
LDFLAGS ?= $(OPT_FLAGS)
CFLAGS ?= -std=c11 $(CFLAGS_BASE) $(LDFLAGS)
CXXFLAGS ?= -std=c++14 $(CFLAGS_BASE) $(LDFLAGS)

###########

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

all: $(TARGET_EXEC)

strip: $(TARGET_EXEC)
	$(STRIP) -s $(TARGET_EXEC)
	$(OBJCOPY) --strip-all $(TARGET_EXEC)

test: strip

compile_commands.json:
	@compiledb make -n all >/dev/null

.PHONY: undefs
undefs:
	@{ \
		grep '^\s*#\s*undef' src/gen_queue.h | sed -e 's@#\s*undef@@g' ; \
		grep '^\s*#\s*define' src/gen_queue.h | sed -e 's@#\s*define@@g' ; \
		grep '^\s*#\s*ifdef' src/gen_queue.h | sed -e 's@#\s*ifdef@@g' ; \
		grep '^\s*#\s*ifndef' src/gen_queue.h | sed -e 's@#\s*ifndef@@g' ; \
	} | awk '{print $$1}' | cut -d'(' -f1 | sort | uniq | while read f ; do echo "#undef $$f" ; done

.PHONY: format_prereq
format: format_prereq
format_prereq: .clang-format .clangd
.clang-format:
	ln -sf $(shell realpath ../../qmk_firmware/.clang-format) .clang-format
.clangd:
	ln -sf $(shell realpath ../../qmk_firmware/.clangd) .clangd

define setup_formatter
format: format_$1
format_$1: $1 format_prereq
	clang-format -i "$1"
endef
$(foreach src,$(SRCS) $(HDRS),$(eval $(call setup_formatter,$(src))))

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(AS) $(call uniq,$(ASFLAGS)) -c $< -o $@

# c sources
$(BUILD_DIR)/%.c.o: %.c compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CC) $(call uniq,$(CPPFLAGS) $(CFLAGS)) -c $< -o $@

# c++ sources
$(BUILD_DIR)/%.cpp.o: %.cpp compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(CPPFLAGS) $(CXXFLAGS)) -c $< -o $@

$(BUILD_DIR)/%.cxx.o: %.cxx compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(CPPFLAGS) $(CXXFLAGS)) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(CPPFLAGS) $(CXXFLAGS)) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) compile_commands.json $(TARGET_EXEC)

.PHONY: distclean

distclean: clean

$(TARGET_EXEC): $(BUILD_DIR)/$(TARGET_EXEC)
	cp -f $(BUILD_DIR)/$(TARGET_EXEC) $(TARGET_EXEC)

-include $(DEPS)

MKDIR_P ?= @mkdir -p