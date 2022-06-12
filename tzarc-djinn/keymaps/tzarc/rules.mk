DEBUG_MATRIX_SCAN_RATE_ENABLE = api

#SRC += qp_rgb565_surface.c

LUA_ENABLE = yes
MOUSEKEY_ENABLE = yes
UNICODE_ENABLE = yes

CONSOLE_ENABLE = yes

ENCODER_MAP_ENABLE = yes

SWAP_HANDS_ENABLE = no

AUDIO_ENABLE = no

VPATH += \
	$(KEYBOARD_PATH_5)/graphics \
	$(KEYBOARD_PATH_5)/graphics/src \
	$(KEYBOARD_PATH_4)/graphics \
	$(KEYBOARD_PATH_4)/graphics/src \
	$(KEYBOARD_PATH_3)/graphics \
	$(KEYBOARD_PATH_3)/graphics/src \
	$(KEYBOARD_PATH_2)/graphics \
	$(KEYBOARD_PATH_2)/graphics/src \
	$(KEYBOARD_PATH_1)/graphics \
	$(KEYBOARD_PATH_1)/graphics/src
