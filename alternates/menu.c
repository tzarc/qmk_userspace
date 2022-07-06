// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include QMK_KEYBOARD_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <printf.h>

#include <quantum.h>
#include <qp.h>
#include <rgb_matrix.h>

#include <process_keycode/process_unicode_common.h>

typedef enum _menu_flags_t {
    menu_flag_is_parent = (1 << 0),
    menu_flag_is_value  = (1 << 1),
} menu_flags_t;

typedef enum _menu_input_t {
    menu_input_exit,
    menu_input_back,
    menu_input_enter,
    menu_input_up,
    menu_input_down,
    menu_input_left,
    menu_input_right,
} menu_input_t;

typedef struct _menu_entry_t {
    menu_flags_t flags;
    const char * text;
    union {
        struct {
            struct _menu_entry_t *children;
            size_t                child_count;
        } parent;
        struct {
            bool (*menu_handler)(menu_input_t input);
            void (*display_handler)(char *text_buffer, size_t buffer_len);
        } child;
    };
} menu_entry_t;

static bool menu_handler_unicode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            cycle_unicode_input_mode(-1);
            return false;
        case menu_input_right:
            cycle_unicode_input_mode(1);
            return false;
        default:
            return true;
    }
}

void display_handler_unicode(char *text_buffer, size_t buffer_len) {
    switch (get_unicode_input_mode()) {
        case UC_MAC:
            strncpy(text_buffer, "macOS", buffer_len - 1);
            return;
        case UC_LNX:
            strncpy(text_buffer, "Linux", buffer_len - 1);
            return;
        case UC_BSD:
            strncpy(text_buffer, "BSD", buffer_len - 1);
            return;
        case UC_WIN:
            strncpy(text_buffer, "Windows", buffer_len - 1);
            return;
        case UC_WINC:
            strncpy(text_buffer, "WinCompose", buffer_len - 1);
            return;
    }

    strncpy(text_buffer, "Unknown", buffer_len);
}

static bool menu_handler_rgbenabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgb_matrix_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbenabled(char *text_buffer, size_t buffer_len) { snprintf_(text_buffer, buffer_len - 1, "%s", rgb_matrix_is_enabled() ? "on" : "off"); }

static bool menu_handler_rgbmode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_step_reverse();
            return false;
        case menu_input_right:
            rgb_matrix_step();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbmode(char *text_buffer, size_t buffer_len) { snprintf_(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_mode()); }

static bool menu_handler_rgbhue(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_hue();
            return false;
        case menu_input_right:
            rgb_matrix_increase_hue();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbhue(char *text_buffer, size_t buffer_len) { snprintf_(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_hue()); }

static bool menu_handler_rgbsat(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_sat();
            return false;
        case menu_input_right:
            rgb_matrix_increase_sat();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbsat(char *text_buffer, size_t buffer_len) { snprintf_(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_sat()); }

static bool menu_handler_rgbval(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_val();
            return false;
        case menu_input_right:
            rgb_matrix_increase_val();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbval(char *text_buffer, size_t buffer_len) { snprintf_(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_val()); }

static bool menu_handler_rgbspeed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_speed();
            return false;
        case menu_input_right:
            rgb_matrix_increase_speed();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbspeed(char *text_buffer, size_t buffer_len) { snprintf_(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_speed()); }

menu_entry_t rgb_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB enabled",
        .child.menu_handler    = menu_handler_rgbenabled,
        .child.display_handler = display_handler_rgbenabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB mode",
        .child.menu_handler    = menu_handler_rgbmode,
        .child.display_handler = display_handler_rgbmode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB hue",
        .child.menu_handler    = menu_handler_rgbhue,
        .child.display_handler = display_handler_rgbhue,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB saturation",
        .child.menu_handler    = menu_handler_rgbsat,
        .child.display_handler = display_handler_rgbsat,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB value",
        .child.menu_handler    = menu_handler_rgbval,
        .child.display_handler = display_handler_rgbval,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB speed",
        .child.menu_handler    = menu_handler_rgbspeed,
        .child.display_handler = display_handler_rgbspeed,
    },
};

menu_entry_t root_entries[] = {
    {.flags = menu_flag_is_value, .text = "Unicode mode", .child.menu_handler = menu_handler_unicode, .child.display_handler = display_handler_unicode},
    {
        .flags              = menu_flag_is_parent,
        .text               = "RGB settings",
        .parent.children    = rgb_entries,
        .parent.child_count = sizeof(rgb_entries) / sizeof(rgb_entries[0]),
    },
};

menu_entry_t root = {
    .flags              = menu_flag_is_parent,
    .text               = "Configuration",
    .parent.children    = root_entries,
    .parent.child_count = sizeof(root_entries) / sizeof(root_entries[0]),
};

typedef struct _menu_state_t {
    bool    dirty;
    bool    is_in_menu;
    uint8_t selected_child;
    uint8_t menu_stack[8];
} menu_state_t;

static menu_state_t state = {.dirty = false, .is_in_menu = false, .menu_stack = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, .selected_child = 0xFF};

menu_entry_t *get_current_menu(void) {
    if (state.selected_child == 0xFF) {
        return NULL;
    }

    menu_entry_t *entry = &root;
    for (int i = 0; i < sizeof(state.menu_stack); ++i) {
        if (state.menu_stack[i] == 0xFF) {
            return entry;
        }
        entry = &entry->parent.children[state.menu_stack[i]];
    }

    return entry;
}

menu_entry_t *get_selected_menu_item(void) { return &(get_current_menu()->parent.children[state.selected_child]); }

bool menu_handle_input(menu_input_t input) {
    menu_entry_t *menu     = get_current_menu();
    menu_entry_t *selected = get_selected_menu_item();
    switch (input) {
        case menu_input_exit:
            state.is_in_menu = false;
            memset(state.menu_stack, 0xFF, sizeof(state.menu_stack));
            state.selected_child = 0xFF;
            return false;
        case menu_input_back:
            // Iterate backwards through the stack and remove the last entry
            for (int i = 0; i < sizeof(state.menu_stack); ++i) {
                if (state.menu_stack[sizeof(state.menu_stack) - 1 - i] != 0xFF) {
                    state.selected_child                               = state.menu_stack[sizeof(state.menu_stack) - 1 - i];
                    state.menu_stack[sizeof(state.menu_stack) - 1 - i] = 0xFF;
                    break;
                }

                // If we've dropped out of the last entry in the stack, exit the menu
                if (i == sizeof(state.menu_stack) - 1) {
                    state.is_in_menu     = false;
                    state.selected_child = 0xFF;
                }
            }
            return false;
        case menu_input_enter:
            // Only attempt to enter the next menu if we're a parent object
            if (selected->flags & menu_flag_is_parent) {
                // Iterate forwards through the stack and add the selected entry
                for (int i = 0; i < sizeof(state.menu_stack); ++i) {
                    if (state.menu_stack[i] == 0xFF) {
                        state.menu_stack[i]  = state.selected_child;
                        state.selected_child = 0;
                        break;
                    }
                }
            }
            return false;
        case menu_input_up:
            state.selected_child = (state.selected_child + menu->parent.child_count - 1) % menu->parent.child_count;
            return false;
        case menu_input_down:
            state.selected_child = (state.selected_child + menu->parent.child_count + 1) % menu->parent.child_count;
            return false;
        case menu_input_left:
        case menu_input_right:
            if (selected->flags & menu_flag_is_value) {
                state.dirty = true;
                return selected->child.menu_handler(input);
            }
            return false;
        default:
            return false;
    }
}

#include "tzarc.h"

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
    if (keycode == DJINN_MENU && record->event.pressed && !state.is_in_menu) {
        state.is_in_menu     = true;
        state.selected_child = 0;
        return false;
    }

    if (state.is_in_menu) {
        if (record->event.pressed) {
            switch (keycode) {
                case DJINN_MENU:
                    return menu_handle_input(menu_input_exit);
                case KC_ESC:
                case KC_GESC:
                    return menu_handle_input(menu_input_back);
                case KC_SPACE:
                case KC_ENTER:
                case KC_RETURN:
                case KC_SFTENT:
                case KC_SFT_ENT:
                    return menu_handle_input(menu_input_enter);
                case KC_UP:
                case KC_W:
                    return menu_handle_input(menu_input_up);
                case KC_DOWN:
                case KC_S:
                    return menu_handle_input(menu_input_down);
                case KC_LEFT:
                case KC_A:
                    return menu_handle_input(menu_input_left);
                case KC_RIGHT:
                case KC_D:
                    return menu_handle_input(menu_input_right);
                default:
                    return false;
            }
        }
        return false;
    }

    return true;
}

#include "thintel15.h"

void render_menu(void) {
    static menu_state_t last_state;
    if (memcmp(&last_state, &state, sizeof(menu_state_t)) == 0) {
        return;
    }

    state.dirty = false;
    memcpy(&last_state, &state, sizeof(menu_state_t));

    if (state.is_in_menu) {
        qp_rect(lcd, 0, 0, 239, 319, 0, 0, 0, true);

        uint8_t       hue      = rgb_matrix_get_hue();
        menu_entry_t *menu     = get_current_menu();
        menu_entry_t *selected = get_selected_menu_item();

        int y = 80;
        qp_rect(lcd, 0, y, 239, y + 3, hue, 255, 255, true);
        y += 8;
        qp_drawtext(lcd, 8, y, font_thintel15, menu->text);
        y += font_thintel15->glyph_height + 4;
        qp_rect(lcd, 0, y, 239, y + 3, hue, 255, 255, true);
        y += 8;
        for (int i = 0; i < menu->parent.child_count; ++i) {
            menu_entry_t *child = &menu->parent.children[i];
            uint16_t      x;
            if (child == selected) {
                x = qp_drawtext_recolor(lcd, 8, y, font_thintel15, child->text, HSV_GREEN, 85, 255, 0);
            } else {
                x = qp_drawtext_recolor(lcd, 8, y, font_thintel15, child->text, HSV_RED, 0, 255, 0);
            }
            if (child->flags & menu_flag_is_parent) {
                qp_drawtext(lcd, 8 + x, y, font_thintel15, "  >");
            }
            if (child->flags & menu_flag_is_value) {
                char buf[32] = {0};
                child->child.display_handler(buf, sizeof(buf));
                qp_drawtext(lcd, 8 + x, y, font_thintel15, buf);
            }
            y += font_thintel15->glyph_height + 4;
            qp_rect(lcd, 0, y, 239, y, hue, 255, 255, true);
            y += 5;
        }
    } else {
        qp_rect(lcd, 0, 0, 239, 319, 0, 0, 0, true);
    }
}