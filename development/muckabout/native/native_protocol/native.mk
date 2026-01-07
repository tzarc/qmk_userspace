NATIVE_DIR = protocol/native

SRC += $(NATIVE_DIR)/native_protocol.c

# Search Path
VPATH += $(TMK_PATH)/$(NATIVE_DIR)

OPT_DEFS += -DPROTOCOL_NATIVE
