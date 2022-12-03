#!/bin/bash
# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

umask 022

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")"

target_branch="nvram-refactor"

cd "$qmk_firmware_dir"

git checkout -f develop
git branch -D $target_branch || true
git checkout -f -b $target_branch develop

if [[ "$target_branch" != "$(git branch --show-current)" ]]; then
    echo "Expected branch '$target_branch', was branch '$(git branch --show-current)'" >&2
    exit 1
fi

git fetch upstream
git fetch upstream --tags
git reset --hard upstream/develop
git clean -xfd || true

string_replace() {
    local from_text=$1
    local to_text=$2
    local location=${3:-.}
    pushd "$location"
    echo -- "-------------------------"
    git grep --name-only "$from_text" | sort | uniq | while read file; do
        echo $file
        sed -i \
            -e "s@$from_text@$to_text@g" \
            "$file" || true
    done || true
    popd
}

convert_one() {
    local from_func=$1
    local to_func=$2
    echo -- "-------------------------"
    git grep --name-only "$from_func" | sort | uniq | while read file; do
        echo $file
        sed -i \
            -e "s@$from_func(\s*@$to_func(@g" \
            -e "s@$to_func(\(.*\)\(void\|uint8_t\|uint16_t\|uint32_t\)\s*\*\s*addr@$to_func(\1uint32_t addr@g" \
            -e "s@$to_func(\(.*\)const\s*uint32_t\s*addr@$to_func(\1uint32_t addr@g" \
            -e "s@$to_func(\s*(\s*uint8_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*uint16_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*uint32_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*void\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*uint8_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*uint16_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*uint32_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*void\s*\*\s*)\s*@$to_func(@g" \
            "$file" || true
    done || true
}

block_reorder() {
    local search_func=$1
    echo -- "-------------------------"
    git grep --name-only "$search_func" | sort | uniq | while read file; do
        echo $file
        sed -i \
            -e "s@$search_func(\s*\\(.*\\)\s*,\s*\\(.*\\)\s*,\s*\\(.*\\)\s*)@$search_func(\2, \1, \3)@g" \
            -e "s@$search_func(\s*(\s*void\s*\*\s*)\s*@$search_func(@g" \
            -e "s@$search_func(\s*(\s*uintptr_t\s*)\s*@$search_func(@g" \
            -e "s@$search_func(\s*(\s*const\s*void\s*\*\s*)\s*@$search_func(@g" \
            "$file" || true
    done || true
}

########################################################################################################################################################################################################

do_stuff() {
    # Remove the EEPROM declarations
    #sed -i '5,24d' platforms/eeprom.h

    cat <<__EOF__ >>platforms/nvram.h
// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

void nvram_read_block(uint32_t addr, void *buf, size_t len);
uint8_t nvram_read_u8(uint32_t addr);
uint16_t nvram_read_u16(uint32_t addr);
uint32_t nvram_read_u32(uint32_t addr);

void nvram_update_block(uint32_t addr, const void *buf, size_t len);
void nvram_update_u8(uint32_t addr, uint8_t value);
void nvram_update_u16(uint32_t addr, uint16_t value);
void nvram_update_u32(uint32_t addr, uint32_t value);

void nvram_write_block(uint32_t addr, const void *buf, size_t len);
void nvram_write_u8(uint32_t addr, uint8_t value);
void nvram_write_u16(uint32_t addr, uint16_t value);
void nvram_write_u32(uint32_t addr, uint32_t value);
__EOF__

    cat <<__EOF__ >>platforms/nvram.c
// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

#include "eeprom.h"

void nvram_read_block(uint32_t addr, void *buf, size_t len) { eeprom_read_block(buf, (const void*)(uintptr_t)addr, len); }
uint8_t nvram_read_u8(uint32_t addr) { return eeprom_read_byte((const void *)(uintptr_t)addr); }
uint16_t nvram_read_u16(uint32_t addr) { return eeprom_read_word((const void *)(uintptr_t)addr); }
uint32_t nvram_read_u32(uint32_t addr) { return eeprom_read_dword((const void *)(uintptr_t)addr); }

void nvram_update_block(uint32_t addr, const void *buf, size_t len) { eeprom_update_block(buf, (void *)(uintptr_t)addr, len); }
void nvram_update_u8(uint32_t addr, uint8_t value) { eeprom_update_byte((void *)(uintptr_t)addr, value); }
void nvram_update_u16(uint32_t addr, uint16_t value) { eeprom_update_word((void *)(uintptr_t)addr, value); }
void nvram_update_u32(uint32_t addr, uint32_t value) { eeprom_update_dword((void *)(uintptr_t)addr, value); }

void nvram_write_block(uint32_t addr, const void *buf, size_t len) { eeprom_write_block(buf, (void *)(uintptr_t)addr, len); }
void nvram_write_u8(uint32_t addr, uint8_t value) { eeprom_write_byte((void *)(uintptr_t)addr, value); }
void nvram_write_u16(uint32_t addr, uint16_t value) { eeprom_write_word((void *)(uintptr_t)addr, value); }
void nvram_write_u32(uint32_t addr, uint32_t value) { eeprom_write_dword((void *)(uintptr_t)addr, value); }
__EOF__

    patch -p1 --ignore-whitespace <<__EOF__
diff --git a/builddefs/common_features.mk b/builddefs/common_features.mk
index 63814bbfae..d67ba860c5 100644
--- a/builddefs/common_features.mk
+++ b/builddefs/common_features.mk
@@ -21,7 +21,7 @@ QUANTUM_SRC += \\
     \$(QUANTUM_DIR)/action_layer.c \\
     \$(QUANTUM_DIR)/action_tapping.c \\
     \$(QUANTUM_DIR)/action_util.c \\
-    \$(QUANTUM_DIR)/eeconfig.c \\
+    \$(QUANTUM_DIR)/nvconfig.c \\
     \$(QUANTUM_DIR)/keyboard.c \\
     \$(QUANTUM_DIR)/keymap_common.c \\
     \$(QUANTUM_DIR)/keycode_config.c \\
diff --git a/platforms/common.mk b/platforms/common.mk
index 693bdc8cf0..f0e1ce0b6d 100644
--- a/platforms/common.mk
+++ b/platforms/common.mk
@@ -2,6 +2,7 @@ PLATFORM_COMMON_DIR = \$(PLATFORM_PATH)/\$(PLATFORM_KEY)

 TMK_COMMON_SRC +=	\\
 	\$(PLATFORM_PATH)/suspend.c \\
+	\$(PLATFORM_PATH)/nvram.c \\
 	\$(PLATFORM_COMMON_DIR)/hardware_id.c \\
 	\$(PLATFORM_COMMON_DIR)/platform.c \\
 	\$(PLATFORM_COMMON_DIR)/suspend.c \\
__EOF__

    git add -A
    git commit -am 'Add NVRAM API header, remove old EEPROM API declarations.'
}
do_stuff

########################################################################################################################################################################################################

do_stuff() {
    string_replace 'include <eeprom.h>' 'include "nvram.h"'
    string_replace 'include "eeprom.h"' 'include "nvram.h"'
    string_replace 'include <eeconfig.h>' 'include "nvconfig.h"'
    string_replace 'include "eeconfig.h"' 'include "nvconfig.h"'

    git mv quantum/eeconfig.c quantum/nvconfig.c
    git mv quantum/eeconfig.h quantum/nvconfig.h
    string_replace 'eeconfig\.' 'nvconfig.'
    string_replace 'eeconfig_' 'nvconfig_'
    string_replace 'EECONFIG_' 'NVCONFIG_'

    patch -p1 --ignore-whitespace <<__EOF__
diff --git a/quantum/nvconfig.h b/quantum/nvconfig.h
index 7c50560a5a..e6e9a7fd41 100644
--- a/quantum/nvconfig.h
+++ b/quantum/nvconfig.h
@@ -26,29 +26,29 @@ along with this program.  If not, see <http://www.gnu.org/licenses/>.
 #define NVCONFIG_MAGIC_NUMBER_OFF (uint16_t)0xFFFF

 /* EEPROM parameter address */
-#define NVCONFIG_MAGIC (uint16_t *)0
-#define NVCONFIG_DEBUG (uint8_t *)2
-#define NVCONFIG_DEFAULT_LAYER (uint8_t *)3
-#define NVCONFIG_KEYMAP (uint16_t *)4
-#define NVCONFIG_BACKLIGHT (uint8_t *)6
-#define NVCONFIG_AUDIO (uint8_t *)7
-#define NVCONFIG_RGBLIGHT (uint32_t *)8
-#define NVCONFIG_UNICODEMODE (uint8_t *)12
-#define NVCONFIG_STENOMODE (uint8_t *)13
+#define NVCONFIG_MAGIC 0
+#define NVCONFIG_DEBUG 2
+#define NVCONFIG_DEFAULT_LAYER 3
+#define NVCONFIG_KEYMAP 4
+#define NVCONFIG_BACKLIGHT 6
+#define NVCONFIG_AUDIO 7
+#define NVCONFIG_RGBLIGHT 8
+#define NVCONFIG_UNICODEMODE 12
+#define NVCONFIG_STENOMODE 13
 // EEHANDS for two handed boards
-#define NVCONFIG_HANDEDNESS (uint8_t *)14
-#define NVCONFIG_KEYBOARD (uint32_t *)15
-#define NVCONFIG_USER (uint32_t *)19
-#define NVCONFIG_VELOCIKEY (uint8_t *)23
+#define NVCONFIG_HANDEDNESS 14
+#define NVCONFIG_KEYBOARD 15
+#define NVCONFIG_USER 19
+#define NVCONFIG_VELOCIKEY 23

-#define NVCONFIG_HAPTIC (uint32_t *)24
+#define NVCONFIG_HAPTIC 24

 // Mutually exclusive
-#define NVCONFIG_LED_MATRIX (uint32_t *)28
-#define NVCONFIG_RGB_MATRIX (uint32_t *)28
+#define NVCONFIG_LED_MATRIX 28
+#define NVCONFIG_RGB_MATRIX 28
 // Speed & Flags
-#define NVCONFIG_LED_MATRIX_EXTENDED (uint16_t *)32
-#define NVCONFIG_RGB_MATRIX_EXTENDED (uint16_t *)32
+#define NVCONFIG_LED_MATRIX_EXTENDED 32
+#define NVCONFIG_RGB_MATRIX_EXTENDED 32

 // Size of EEPROM being used for core data storage
 #define NVCONFIG_BASE_SIZE 34
@@ -67,8 +67,8 @@ along with this program.  If not, see <http://www.gnu.org/licenses/>.
 #    define NVCONFIG_USER_DATA_VERSION (NVCONFIG_USER_DATA_SIZE)
 #endif

-#define NVCONFIG_KB_DATABLOCK ((uint8_t *)(NVCONFIG_BASE_SIZE))
-#define NVCONFIG_USER_DATABLOCK ((uint8_t *)((NVCONFIG_BASE_SIZE) + (NVCONFIG_KB_DATA_SIZE)))
+#define NVCONFIG_KB_DATABLOCK (NVCONFIG_BASE_SIZE)
+#define NVCONFIG_USER_DATABLOCK ((NVCONFIG_BASE_SIZE) + (NVCONFIG_KB_DATA_SIZE)))

 // Size of EEPROM being used, other code can refer to this for available EEPROM
 #define NVCONFIG_SIZE ((NVCONFIG_BASE_SIZE) + (NVCONFIG_KB_DATA_SIZE) + (NVCONFIG_USER_DATA_SIZE))
__EOF__

    git checkout -- platforms/nvram.* drivers/ platforms/
    git add -A
    git commit -am 'Convert `eeprom` => `nvram`, and `eeconfig` => `nvconfig`.'
}
do_stuff

########################################################################################################################################################################################################

do_stuff() {
    convert_one eeprom_read_byte nvram_read_u8
    convert_one eeprom_write_byte nvram_write_u8
    convert_one eeprom_update_byte nvram_update_u8
    git checkout -- platforms/nvram.* drivers/ platforms/
    git add -A
    git commit -am 'Convert `uint8_t` APIs.'
}
do_stuff

########################################################################################################################################################################################################

do_stuff() {
    convert_one eeprom_read_word nvram_read_u16
    convert_one eeprom_write_word nvram_write_u16
    convert_one eeprom_update_word nvram_update_u16
    git checkout -- platforms/nvram.* drivers/ platforms/
    git add -A
    git commit -am 'Convert `uint16_t` APIs.'
}
do_stuff

########################################################################################################################################################################################################

do_stuff() {
    convert_one eeprom_read_dword nvram_read_u32
    convert_one eeprom_write_dword nvram_write_u32
    convert_one eeprom_update_dword nvram_update_u32
    git checkout -- platforms/nvram.* drivers/ platforms/
    git add -A
    git commit -am 'Convert `uint32_t` APIs.'
}
do_stuff

########################################################################################################################################################################################################

do_stuff() {
    convert_one eeprom_read_block nvram_read_block
    block_reorder nvram_read_block
    convert_one eeprom_write_block nvram_write_block
    block_reorder nvram_write_block
    convert_one eeprom_update_block nvram_update_block
    block_reorder nvram_update_block
    git checkout -- platforms/nvram.* drivers/ platforms/
    git add -A
    git commit -am 'Convert block APIs.'
}
do_stuff

########################################################################################################################################################################################################
exit 0

git mv drivers/eeprom drivers/nvram
git mv platforms/chibios/drivers/eeprom platforms/chibios/drivers/nvram

git reset -- platforms/eeprom.h
git checkout -- platforms/eeprom.h

#git mv quantum/eeconfig.c quantum/nvconfig.c
#git mv quantum/eeconfig.h quantum/nvconfig.h
#string_replace 'eeconfig\.' 'nvconfig.'
#string_replace 'eeconfig_' 'nvconfig_'
#string_replace 'EECONFIG_' 'NVCONFIG_'

git mv drivers/nvram/eeprom_driver.c platforms/nvram_common.c
git mv drivers/nvram/eeprom_driver.h platforms/nvram_common.h
string_replace 'eeprom_driver' 'nvram_driver'
string_replace 'EEPROM_CUSTOM' 'NVRAM_CUSTOM'
string_replace 'EEPROM_DRIVER' 'NVRAM_DRIVER'
string_replace 'DNVRAM_DRIVER' 'DNVRAM_COMMON'
string_replace 'defined(NVRAM_DRIVER)' 'defined(NVRAM_COMMON)'
string_replace 'ifdef NVRAM_DRIVER' 'ifdef NVRAM_COMMON'
string_replace 'nvram_driver\.h' 'nvram_common.h'
string_replace 'nvram_driver\.c' 'nvram_common.c'

git mv drivers/nvram/eeprom_transient.c drivers/nvram/nvram_transient.c
git mv drivers/nvram/eeprom_transient.h drivers/nvram/nvram_transient.h
string_replace 'eeprom_transient' 'nvram_transient'
string_replace 'EEPROM_TRANSIENT' 'NVRAM_TRANSIENT'

git mv drivers/nvram/eeprom_custom.c-template drivers/nvram/nvram_custom.c-template

git ls-files | grep 'eeprom_driver\.md' | while read file; do
    git reset -- $file
    git checkout -- $file
    git mv $file $(dirname $file)/nvram_driver.md
done
# TODO: modify docs/nvram_driver.md

string_replace 'not a valid EEPROM driver' 'not a valid NVRAM driver'

string_replace 'EEPROM' 'NVRAM' docs
string_replace 'eeprom-storage' 'nvram-storage' docs
string_replace 'I2C NVRAM' 'I2C EEPROM' docs
string_replace 'I2C-based NVRAM' 'I2C-based EEPROM' docs
string_replace 'SPI NVRAM' 'SPI EEPROM' docs
string_replace 'SPI-based NVRAM' 'SPI-based EEPROM' docs
string_replace 'NONVRAM' 'noeeprom' docs
string_replace 'QK_CLEAR_NVRAM' 'QK_CLEAR_EEPROM' docs
string_replace 'NVRAM_DRIVER' 'EEPROM_DRIVER' docs
string_replace 'EXTERNAL_NVRAM' 'EXTERNAL_EEPROM' docs
string_replace 'internal NVRAM' 'internal EEPROM' docs
string_replace 'these NVRAM files' 'these EEPROM files' docs
string_replace 'true NVRAM' 'true EEPROM' docs
string_replace 'Reset NVRAM' 'Reset EEPROM' docs
string_replace 'NVRAM chips' 'EEPROM chips' docs

string_replace 'stored in the NVRAM after' 'stored in the EEPROM after' docs
string_replace 'stored in the NVRAM of your controller' 'stored in the NVRAM of your keyboard' docs
string_replace 'shorten the life of your MCU\.' "shorten the life of your board's persistent storage." docs
string_replace 'sets the handedness setting in NVRAM\.' 'sets the handedness setting in NVRAM (requires internal MCU NVRAM to be used).'

git reset -- docs/ChangeLog docs/faq_misc.md docs/fuse.txt
git checkout -- docs/ChangeLog docs/faq_misc.md docs/fuse.txt

string_replace 'drivers/eeprom' 'drivers/nvram'
string_replace '_noeeprom' '_no_nvram'

string_replace '/eeprom$' '/nvram'

cat <<__EOF__ >>platforms/nvram_common.h

uint8_t nvram_read_u8(uint32_t addr);
uint16_t nvram_read_u16(uint32_t addr);
uint32_t nvram_read_u32(uint32_t addr);
void nvram_read_block(uint32_t addr, void *buf, uint32_t len);

void nvram_write_u8(uint32_t addr, uint8_t value);
void nvram_write_u16(uint32_t addr, uint16_t value);
void nvram_write_u32(uint32_t addr, uint32_t value);
void nvram_write_block(uint32_t addr, const void *buf, size_t len);

void nvram_update_u8(uint32_t addr, uint8_t value);
void nvram_update_u16(uint32_t addr, uint16_t value);
void nvram_update_u32(uint32_t addr, uint32_t value);
void nvram_update_block(uint32_t addr, const void *buf, size_t len);
__EOF__

git add -A
git diff --cached
