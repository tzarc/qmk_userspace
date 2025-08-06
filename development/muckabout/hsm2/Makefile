# Copyright 2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
TARGET_EXEC ?= hsm
TEST_EXEC ?= test_hsm

BUILD_DIR ?= .build
TEST_BUILD_DIR ?= .build/tests
SRC_DIRS ?= src
TEST_DIRS ?= tests

SRCS := $(shell find $(SRC_DIRS) \( -name *.cpp -or -name *.cxx -or -name *.cc -or -name *.c -or -name *.s -or -name *.S \))
HDRS := $(shell find $(SRC_DIRS) \( -name *.hpp -or -name *.hxx -or -name *.hh -or -name *.h \))
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TEST_SRCS := $(shell find $(TEST_DIRS) \( -name *.cpp -or -name *.cxx -or -name *.cc -or -name *.c -or -name *.s -or -name *.S \))
LIB_SRCS := $(filter-out src/main.c, $(SRCS))
LIB_OBJS := $(LIB_SRCS:%=$(BUILD_DIR)/%.o)
LIB_TEST_OBJS := $(LIB_SRCS:%=$(TEST_BUILD_DIR)/%.o)
TEST_OBJS := $(TEST_SRCS:%=$(TEST_BUILD_DIR)/%.o)
TEST_DEPS := $(TEST_OBJS:.o=.d) $(LIB_TEST_OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
TEST_INC_DIRS := $(shell find $(TEST_DIRS) -type d 2>/dev/null || true)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
TEST_INC_FLAGS := $(INC_FLAGS) $(addprefix -I,$(TEST_INC_DIRS))

TOOLCHAIN_PREFIX =
CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)g++
STRIP = $(TOOLCHAIN_PREFIX)strip
READELF = $(TOOLCHAIN_PREFIX)readelf
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump
SIZE = $(TOOLCHAIN_PREFIX)size

MKDIR_P ?= @mkdir -p

OPT_FLAGS = -g -Og
CFLAGS_BASE ?= $(OPT_FLAGS) -fsanitize=address -fno-omit-frame-pointer

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
TEST_CPPFLAGS ?= $(TEST_INC_FLAGS) -MMD -MP
LDFLAGS ?= $(OPT_FLAGS) -fsanitize=address -fno-omit-frame-pointer
CFLAGS ?= -std=c11 $(CFLAGS_BASE)
CXXFLAGS ?= -std=c++14 $(CFLAGS_BASE)
TEST_CFLAGS ?= $(CFLAGS) --coverage
TEST_CXXFLAGS ?= $(CXXFLAGS) --coverage
TEST_LDFLAGS ?= $(LDFLAGS) --coverage

###########

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

all: $(TARGET_EXEC)

test: $(TEST_EXEC)
	./$(TEST_EXEC)

analyze:
	scan-build make all

coverage: $(TEST_BUILD_DIR)/$(TEST_EXEC)
	$(MKDIR_P) $(BUILD_DIR)/coverage
	./$(TEST_BUILD_DIR)/$(TEST_EXEC)
	gcov -o $(TEST_BUILD_DIR)/src $(TEST_BUILD_DIR)/src/hsm.c.o
	mv *.gcov $(BUILD_DIR)/coverage/ 2>/dev/null || true
	@echo "Coverage files generated in $(BUILD_DIR)/coverage/ directory"
	@echo "View $(BUILD_DIR)/coverage/hsm.c.gcov for detailed line-by-line coverage"

strip: $(TARGET_EXEC)
	$(STRIP) -s $(TARGET_EXEC)
	$(OBJCOPY) --strip-all $(TARGET_EXEC)

compile_commands.json:
	@compiledb make -n all >/dev/null

define setup_formatter
format: format_$1
format_$1: $1
	clang-format -i "$1"
endef
$(foreach src,$(SRCS) $(HDRS) $(TEST_SRCS),$(eval $(call setup_formatter,$(src))))

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(TEST_BUILD_DIR)/$(TEST_EXEC): $(TEST_OBJS) $(LIB_TEST_OBJS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(TEST_OBJS) $(LIB_TEST_OBJS) -o $@ $(TEST_LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(AS) $(call uniq,$(ASFLAGS)) -c $< -o $@

$(BUILD_DIR)/%.S.o: %.s compile_commands.json
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

# test c sources
$(TEST_BUILD_DIR)/%.c.o: %.c compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CC) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CFLAGS)) -c $< -o $@

# test c++ sources
$(TEST_BUILD_DIR)/%.cpp.o: %.cpp compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CXXFLAGS)) -c $< -o $@

$(TEST_BUILD_DIR)/%.cxx.o: %.cxx compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CXXFLAGS)) -c $< -o $@

$(TEST_BUILD_DIR)/%.cc.o: %.cc compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CXXFLAGS)) -c $< -o $@

# library sources for testing (with coverage)
$(TEST_BUILD_DIR)/src/%.c.o: src/%.c compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CC) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CFLAGS)) -c $< -o $@

$(TEST_BUILD_DIR)/src/%.cpp.o: src/%.cpp compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CXXFLAGS)) -c $< -o $@

$(TEST_BUILD_DIR)/src/%.cxx.o: src/%.cxx compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CXXFLAGS)) -c $< -o $@

$(TEST_BUILD_DIR)/src/%.cc.o: src/%.cc compile_commands.json
	$(MKDIR_P) $(dir $@)
	$(CXX) $(call uniq,$(TEST_CPPFLAGS) $(TEST_CXXFLAGS)) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) compile_commands.json $(TARGET_EXEC) $(TEST_EXEC) *.gcda *.gcno

.PHONY: distclean

distclean: clean

$(TARGET_EXEC): $(BUILD_DIR)/$(TARGET_EXEC)
	cp -f $(BUILD_DIR)/$(TARGET_EXEC) $(TARGET_EXEC)

$(TEST_EXEC): $(TEST_BUILD_DIR)/$(TEST_EXEC)
	cp -f $(TEST_BUILD_DIR)/$(TEST_EXEC) $(TEST_EXEC)

-include $(DEPS)
-include $(TEST_DEPS)
