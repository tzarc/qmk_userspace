# QMK Native Linux Platform Implementation Plan

**Classification**: Healthcare and Defense Application
**Criticality**: High - Lives may depend on reliable operation
**Version**: 2.0
**Date**: 2026-02-02

---

## 1. Overview

### 1.1 Objective

Implement a native Linux platform backend for QMK firmware that:

1. Runs as a userspace application on Linux systems
2. Interfaces with physical GPIO via the Linux GPIO subsystem (libgpiod v2)
3. Presents as a USB HID device via the Linux USB Gadget subsystem
4. Maintains full compatibility with existing QMK keyboard definitions

### 1.2 Target Hardware

- Linux systems with GPIO (Raspberry Pi, BeagleBone, industrial SBCs)
- Systems with USB Device Controller (UDC) for USB Gadget
- Minimum kernel version: 5.10

### 1.3 Critical Requirements

| ID | Requirement | Priority |
|----|-------------|----------|
| REQ-001 | GPIO operations complete within 100µs | MANDATORY |
| REQ-002 | USB HID reports delivered within 1ms of key event | MANDATORY |
| REQ-003 | Graceful handling of GPIO/USB failures | MANDATORY |
| REQ-004 | All errors logged with timestamps | MANDATORY |
| REQ-005 | No memory leaks under continuous operation | MANDATORY |
| REQ-006 | Clean shutdown on SIGTERM/SIGINT | MANDATORY |
| REQ-007 | Runtime configuration via file | MANDATORY |

---

## 2. Architecture

### 2.1 Component Diagram

```
┌─────────────────────────────────────────┐
│         QMK Firmware Core               │
│    (quantum/, tmk_core/ - unchanged)    │
└───────────────┬─────────────────────────┘
                │
    ┌───────────┴───────────┐
    ▼                       ▼
┌─────────────┐     ┌──────────────┐
│  Platform   │     │   Protocol   │
│  (native/)  │     │   (native/)  │
│             │     │              │
│ • GPIO      │     │ • USB Gadget │
│ • Timer     │     │ • HID reports│
│ • EEPROM    │     │ • LED state  │
└──────┬──────┘     └──────┬───────┘
       │                   │
       ▼                   ▼
┌─────────────┐     ┌──────────────┐
│  libgpiod   │     │ /dev/hidgN   │
│  /dev/gpio* │     │  ConfigFS    │
└─────────────┘     └──────────────┘
```

### 2.2 Threading Model

Single-threaded event loop matching QMK's existing model. No mutexes required for keyboard state.

### 2.3 Startup Sequence

1. `platform_setup()` - Initialize logging, GPIO, timer; install signal handlers
2. `protocol_setup()` - Configure USB Gadget via ConfigFS
3. `protocol_pre_init()` - Register `host_driver_t`, open `/dev/hidgN`
4. Main loop until shutdown signal
5. `native_cleanup()` - Release GPIO, close gadget, flush logs

---

## 3. Directory Structure

### 3.1 Platform Files (`native_platform/` → `platforms/native/`)

| File | Purpose |
|------|---------|
| `mcu_selection.mk` | Build system integration, sets `PLATFORM_KEY=native` |
| `platform.mk` | Compiler flags, library linking (`-lgpiod -lpthread`) |
| `platform.c` | `platform_setup()`, signal handlers, cleanup |
| `platform_deps.h` | Common includes for platform code |
| `gpio.h` | GPIO function declarations |
| `gpio.c` | GPIO implementation using libgpiod v2 |
| `_pin_defs.h` | `pin_t` type definition and encoding macros |
| `_wait.h` | `wait_ms()`, `wait_us()` using `nanosleep()` |
| `timer.c` | Timer using `clock_gettime(CLOCK_MONOTONIC)` |
| `atomic_util.h` | `ATOMIC_BLOCK` macro (no-op for single-threaded) |
| `suspend.c` | No-op stubs for power management |
| `eeprom.c` | File-backed EEPROM emulation |
| `bootloaders/none.c` | Empty bootloader stub |

### 3.2 Protocol Files (`native_protocol/` → `tmk_core/protocol/native/`)

| File | Purpose |
|------|---------|
| `native.mk` | Protocol build rules |
| `native_protocol.h` | Public API, `extern host_driver_t native_driver` |
| `native_protocol.c` | `host_driver_t` implementation, lifecycle functions |
| `usb_gadget.h` | USB Gadget abstraction API |
| `usb_gadget.c` | ConfigFS setup, `/dev/hidgN` I/O |

**Note**: HID report descriptors reused from `tmk_core/protocol/usb_descriptor.c` - no duplication.

---

## 4. Platform Specifications

### 4.1 `mcu_selection.mk`

**Trigger**: `MCU` contains "native"

**Must set**:
- `PLATFORM_KEY = native`
- `PROTOCOL = native`
- `BOOTLOADER = none`
- `F_CPU = 0`

### 4.2 `platform.mk`

**Compiler**: GCC for host architecture

**Required flags**:
- `-Wall -Wextra -Werror` (strict warnings)
- `-std=gnu11`
- Link: `-lgpiod -lpthread -lrt`

**Pkg-config**: Use `pkg-config --cflags --libs libgpiod` if available

### 4.3 `_pin_defs.h`

**Type**: `typedef uint16_t pin_t`

**Encoding**:
- High byte: GPIO chip index (0-255)
- Low byte: Line offset (0-255)

**Required macros**:
- `NATIVE_PIN(chip, offset)` - Encode pin
- `NATIVE_PIN_CHIP(pin)` - Extract chip index
- `NATIVE_PIN_OFFSET(pin)` - Extract line offset

### 4.4 `gpio.h` / `gpio.c`

**Dependencies**: libgpiod v2 API

**Required functions** (see `platforms/gpio.h` for signatures):
- `gpio_set_pin_input(pin)`
- `gpio_set_pin_input_high(pin)` - With pull-up bias
- `gpio_set_pin_input_low(pin)` - With pull-down bias
- `gpio_set_pin_output_push_pull(pin)`
- `gpio_set_pin_output_open_drain(pin)`
- `gpio_write_pin_high(pin)`
- `gpio_write_pin_low(pin)`
- `gpio_write_pin(pin, level)`
- `gpio_read_pin(pin)` - Returns `bool`
- `gpio_toggle_pin(pin)`

**Additional lifecycle functions**:
- `native_gpio_init()` - Open GPIO chips, returns `bool`
- `native_gpio_shutdown()` - Release all lines and chips

**Behavior**:
- Discover chips at `/dev/gpiochip0`, `/dev/gpiochip1`, etc.
- Track line requests per pin for reconfiguration
- Invalid pin (`NO_PIN` or out of range): log error, no crash
- libgpiod failure: log with errno, return gracefully

**Performance**: Each GPIO operation < 100µs

### 4.5 `_wait.h`

**Required macros/functions**:
- `wait_ms(uint32_t ms)` - Using `nanosleep()`, handle `EINTR`
- `wait_us(uint32_t us)` - Using `nanosleep()`, handle `EINTR`
- `waitInputPinDelay()` - Minimal delay (1µs) for GPIO settling

### 4.6 `timer.c`

**Clock source**: `CLOCK_MONOTONIC` via `clock_gettime()`

**Required functions** (see `platforms/timer.h`):
- `timer_init()` - Record start time
- `timer_clear()` - Reset start time
- `timer_read()` - Returns `uint16_t` milliseconds
- `timer_read32()` - Returns `uint32_t` milliseconds
- `timer_elapsed(uint16_t last)` - Milliseconds since `last`
- `timer_elapsed32(uint32_t last)` - Milliseconds since `last`

**Precision**: Within 1ms of actual elapsed time

### 4.7 `eeprom.c`

**Backing store**: File at configurable path (default `/var/lib/qmk/eeprom.bin`)

**Required functions** (standard EEPROM API):
- `eeprom_read_byte`, `eeprom_write_byte`
- `eeprom_read_word`, `eeprom_write_word`
- `eeprom_read_dword`, `eeprom_write_dword`
- `eeprom_read_block`, `eeprom_write_block`
- `eeprom_update_*` variants

**Behavior**:
- Initialize buffer to `0xFF` (erased state)
- Load from file on first access
- Atomic writes: write to `.tmp`, then `rename()`
- Create parent directory if needed
- Permission failure: log warning, continue with RAM-only

### 4.8 `platform.c`

**Required function**: `void platform_setup(void)`

**Behavior**:
1. Initialize logging subsystem
2. Install signal handlers for `SIGINT`, `SIGTERM`
3. Call `native_gpio_init()` - exit on failure
4. Call `timer_init()`
5. Log startup with PID

**Additional function**: `bool native_shutdown_requested(void)`
- Returns true after signal received

**Additional function**: `void native_cleanup(void)`
- Call `native_gpio_shutdown()`
- Shutdown logging

### 4.9 `atomic_util.h`

Single-threaded model - provide no-op macros for API compatibility:
- `ATOMIC_BLOCK(type)` - Simple `for` loop executing once
- `ATOMIC_FORCEON`, `ATOMIC_RESTORESTATE` - Empty defines

---

## 5. Protocol Specifications

### 5.1 `native_protocol.h`

**Exports**:
- `extern host_driver_t native_driver`
- `bool native_shutdown_requested(void)` (may be in platform)

### 5.2 `native_protocol.c`

**Implements `host_driver_t`**:
- `keyboard_leds()` - Return cached LED state from gadget
- `send_keyboard(report_keyboard_t*)` - Write to keyboard hidg device
- `send_nkro(report_nkro_t*)` - Write to keyboard hidg device
- `send_mouse(report_mouse_t*)` - Write to mouse hidg device
- `send_extra(report_extra_t*)` - Write to keyboard hidg device (with report ID)

**Protocol lifecycle functions**:
- `protocol_setup()` - Minimal setup
- `protocol_pre_init()` - Initialize USB Gadget, call `host_set_driver(&native_driver)`
- `protocol_post_init()` - Log completion
- `protocol_pre_task()` - Poll gadget for LED state updates
- `protocol_post_task()` - No-op

### 5.3 `usb_gadget.h` / `usb_gadget.c`

**API**:
- `bool usb_gadget_init(void)` - Configure gadget via ConfigFS, open devices
- `void usb_gadget_shutdown(void)` - Unbind gadget, close devices
- `void usb_gadget_poll(void)` - Non-blocking check for LED state
- `bool usb_gadget_send_keyboard_report(const void*, size_t)`
- `bool usb_gadget_send_mouse_report(const void*, size_t)`
- `bool usb_gadget_send_extra_report(const void*, size_t)`
- `uint8_t usb_gadget_get_keyboard_leds(void)`
- `bool usb_gadget_is_connected(void)`

**ConfigFS Setup** (in `usb_gadget_init()`):
1. Create gadget at `/sys/kernel/config/usb_gadget/qmk/`
2. Write VID/PID from QMK's USB configuration
3. Create HID function(s)
4. Write report descriptors using `get_usb_descriptor()` from `usb_descriptor.c`
5. Create configuration, link functions
6. Find UDC in `/sys/class/udc/`, bind gadget

**Device paths**: `/dev/hidg0` (keyboard), `/dev/hidg1` (mouse) - configurable

**Error handling**:
- ConfigFS failure: log error, return false (app continues without USB)
- Write failure: log warning, return false
- `EAGAIN`/`EWOULDBLOCK`: log, return false (host not reading)

**Privilege handling**:
- Requires root for ConfigFS setup
- After gadget bound, can drop privileges
- Implement privilege drop with `setgid()`/`setuid()` to `qmk` user

---

## 6. Error Handling

### 6.1 Error Classification

| Severity | Response |
|----------|----------|
| FATAL | Log, clean shutdown, exit non-zero |
| ERROR | Log with errno, continue if possible |
| WARN | Log, continue normally |

### 6.2 Fatal Conditions

- No GPIO chips found
- Cannot initialize logging

### 6.3 Recoverable Errors

- Individual GPIO line request failure: skip pin, log, continue
- USB Gadget unavailable: log, continue (useful for testing without USB)
- EEPROM file permission denied: use RAM-only, log warning
- USB write fails: log, retry on next report

### 6.4 Logging Requirements

All log entries must include:
- ISO 8601 timestamp with milliseconds
- Log level (ERROR/WARN/INFO/DEBUG)
- Category (SYSTEM/GPIO/USB/TIMER/EEPROM)
- Source file, line, function
- Message with errno when applicable

---

## 7. Testing Requirements

### 7.1 Unit Tests

| Test | Criteria |
|------|----------|
| Pin encoding | `NATIVE_PIN`/`CHIP`/`OFFSET` round-trip |
| Timer accuracy | `timer_read32()` within 1ms of actual |
| Timer overflow | 32-bit wrap handled correctly |
| EEPROM persistence | Values survive simulated restart |

### 7.2 Integration Tests

| Test | Criteria |
|------|----------|
| GPIO output | Physical LED toggles |
| GPIO input | Physical button press detected |
| USB HID send | Host receives valid report (verify with `usbhid-dump`) |
| LED state | Caps Lock state read correctly |
| Matrix scan | Key detected in < 1ms |
| End-to-end | Physical key press → character on host |

### 7.3 Stress Tests

| Test | Criteria |
|------|----------|
| 24-hour run | No memory leaks, no crashes |
| Rapid input | 1000 keys/second processed correctly |
| USB reconnect | Recovery within 1 second |

### 7.4 Performance Validation

- [ ] Matrix scan < 100µs
- [ ] Key-to-host latency < 1ms
- [ ] CPU usage < 5% at idle
- [ ] Memory stable over 24 hours

---

## 8. Security

### 8.1 Privilege Model

1. Start as root (required for ConfigFS)
2. Configure USB Gadget
3. Drop to `qmk` user/group
4. Continue with minimal privileges

### 8.2 Required Permissions

| Resource | Access | Method |
|----------|--------|--------|
| `/dev/gpiochip*` | R/W | udev rule or `gpio` group |
| `/dev/hidg*` | R/W | udev rule after gadget creation |
| `/sys/kernel/config/` | R/W | Root during setup only |
| `/var/lib/qmk/` | R/W | `qmk` user ownership |

### 8.3 Recommended udev Rules

```
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", GROUP="gpio", MODE="0660"
KERNEL=="hidg*", GROUP="qmk", MODE="0660"
```

---

## 9. Implementation Sequence

### Phase 1: Platform Foundation
1. `mcu_selection.mk`
2. `platform.mk`
3. `platform_deps.h`
4. `_pin_defs.h`

**Gate**: Build system recognizes `MCU=native`

### Phase 2: Core Platform
1. `timer.c` and `_wait.h`
2. `atomic_util.h`
3. `platform.c` (without GPIO)

**Gate**: Timer functions return correct values

### Phase 3: GPIO
1. `gpio.h` and `gpio.c`
2. Update `platform.c` to init GPIO

**Gate**: Can toggle physical LED

### Phase 4: Supporting
1. `suspend.c`
2. `eeprom.c`
3. `bootloaders/none.c`

**Gate**: Platform compiles and links completely

### Phase 5: Protocol
1. `native.mk`
2. `usb_gadget.h` and `usb_gadget.c`
3. `native_protocol.h` and `native_protocol.c`

**Gate**: HID reports reach host

### Phase 6: Integration
1. Create test keyboard definition
2. End-to-end validation
3. Stress testing

**Gate**: All tests pass

---

## 10. Configuration

### 10.1 Compile-Time (`native_config.h`)

| Define | Default | Description |
|--------|---------|-------------|
| `NATIVE_GPIO_MAX_CHIPS` | 4 | Max GPIO chips |
| `NATIVE_GPIO_CONSUMER` | "qmk-native" | Consumer name in debugfs |
| `NATIVE_EEPROM_SIZE` | 1024 | EEPROM size in bytes |
| `NATIVE_LOG_LEVEL` | 3 (INFO) | Default log verbosity |

### 10.2 Runtime (`/etc/qmk/native.conf`)

| Key | Description |
|-----|-------------|
| `eeprom_path` | EEPROM backing file |
| `log_path` | Log file path |
| `log_level` | Runtime log level |
| `hidg_keyboard` | Keyboard device path |
| `hidg_mouse` | Mouse device path |

---

## 11. Systemd Integration

```ini
[Unit]
Description=QMK Native Keyboard
After=sys-kernel-config.mount
Requires=sys-kernel-config.mount

[Service]
Type=simple
ExecStart=/usr/local/bin/qmk-native
Restart=always
RestartSec=1
ProtectHome=true
PrivateTmp=true
ReadWritePaths=/var/lib/qmk /var/log /sys/kernel/config/usb_gadget

[Install]
WantedBy=multi-user.target
```

Application handles privilege drop internally after ConfigFS setup.

---

## 12. References

- QMK `platforms/test/` - Minimal platform reference
- QMK `platforms/chibios/gpio.h` - GPIO API reference
- QMK `tmk_core/protocol/host_driver.h` - Protocol interface
- QMK `tmk_core/protocol/usb_descriptor.c` - HID descriptors (reuse, don't duplicate)
- libgpiod v2 documentation
- Linux USB Gadget ConfigFS documentation

---

*End of Plan*
