# STM32F411 BlackPill USB HID NKRO Proof-of-Concept

**Claude-generated. Here be dragons.**

Validates NKRO (N-Key Rollover) USB HID descriptors work correctly with both boot protocol (BIOS) and NKRO-aware hosts (modern OS) using the bitfield workaround technique from [devever.net/~hl/usbnkro](https://www.devever.net/~hl/usbnkro).

**Hardware:** STM32F411CEU6 "BlackPill" board with ChibiOS/RT

## Build & Flash

```bash
make                                              # Build firmware
st-flash write ./usb_hid_keyboard.bin 0x8000000  # Flash via ST-Link
```

## Behavior

**Test:** Button on PA0 sends 'a' keystroke (keycode 0x04)
- Press → key down
- Release → key up
- Works during USB suspend (triggers remote wakeup)

## NKRO Implementation

**Key Technical Decision:** Bytes 2-7 of the HID report (boot_keys) are marked as **Constant with NO keyboard usages** in the descriptor (see usbcfg.c).

**Why:**
- Boot protocol hosts (BIOS) ignore the descriptor and read bytes 0-7 directly
- NKRO hosts (modern OS) parse the descriptor and skip constant fields with no usages
- Firmware populates both boot_keys (bytes 2-7) and NKRO bitfield (bytes 8-20)
- This prevents duplicate keypresses on NKRO hosts while maintaining BIOS compatibility

**Report:** 21 bytes total - 8 bytes boot protocol + 13 bytes NKRO bitfield (104 keys)

## References

- [NKRO Bitfield Workaround](https://www.devever.net/~hl/usbnkro) - Original technique documentation
- [USB HID Specification](https://www.usb.org/hid) - USB HID 1.11
- [ChibiOS](http://www.chibios.org/dokuwiki/doku.php) - Apache License 2.0
