# prime_plus

![Prime+](https://i.imgur.com/QhNHmwxh.png)

The original firmware was read protected, so there's likely no going back to the manufacturers firmware. This QMK firmware does not support the dedicated lift off sensor and only has basic OLED support.

Erased and flashed with stm32duino bootloader adjusted for 12MHz HSE using stlink. A15 is connected to the USB D+ pullup (1.5K).

* Keyboard Maintainer: [Dasky](https://github.com/Dasky)
* Hardware Supported: SteelSeries Prime+
* Hardware Availability: https://steelseries.com/gaming-mice/prime-plus

Make example for this keyboard (after setting up your build environment):

    make prime_plus:default

Flashing example for this keyboard:

    make prime_plus:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down left mouse button and plug in.
* **Bootloader entry**: Hold the CPI while plugging in.
