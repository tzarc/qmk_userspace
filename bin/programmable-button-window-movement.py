#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "dbus-next",
#     "evdev-binary"
# ]
# ///

import sys
import asyncio
import json
import evdev
from dbus_next.message import Message, MessageType
from dbus_next.aio import MessageBus


async def invoke_kwin_method(bus, method_name, signature="", body=[]):
    reply = await bus.call(
        Message(
            destination="org.kde.kglobalaccel",
            path="/component/kwin",
            interface="org.kde.kglobalaccel.Component",
            member=method_name,
            signature=signature,
            body=body,
        )
    )
    return reply


async def get_window_movement_shortcuts(bus):
    reply = await invoke_kwin_method(bus, method_name="shortcutNames")

    if reply.message_type == MessageType.ERROR:
        raise Exception(reply.body)

    print(json.dumps(reply.body, indent=2))


async def invoke_kwin_shortcut(bus, shortcut_name):
    reply = await invoke_kwin_method(
        bus, method_name="invokeShortcut", signature="s", body=[shortcut_name]
    )

    if reply.message_type == MessageType.ERROR:
        raise Exception(reply.body)


programmable_button_mapping = {
    "KEY_MACRO1": "Window to Previous Screen",  # QMK PB_1
    "KEY_MACRO2": "Window to Next Screen",  # QMK PB_2
    "KEY_MACRO3": "Window Maximize",  # QMK PB_3
    "KEY_MACRO4": "Window Quick Tile Left",  # QMK PB_4
    "KEY_MACRO5": "Window Quick Tile Right",  # QMK PB_5
    "KEY_MACRO6": "Window Quick Tile Top",  # QMK PB_6
    "KEY_MACRO7": "Window Quick Tile Bottom",  # QMK PB_7
    "KEY_MACRO8": "Window Quick Tile Top Left",  # QMK PB_8
    "KEY_MACRO9": "Window Quick Tile Top Right",  # QMK PB_9
    "KEY_MACRO10": "Window Quick Tile Bottom Left",  # QMK PB_10
    "KEY_MACRO11": "Window Quick Tile Bottom Right",  # QMK PB_11
}


# Work out which keys to listen for
listen_for = dict(
    filter(lambda e: e[0].startswith("KEY_MACRO"), evdev.ecodes.ecodes.items())
)


async def listen_programmable_buttons_device(bus, device):
    async for event in device.async_read_loop():
        if isinstance(event, evdev.events.InputEvent):
            if event.code in listen_for.values():
                if event.value == 1:  # Key pressed
                    if evdev.ecodes.KEY[event.code] in programmable_button_mapping:
                        shortcut = programmable_button_mapping[
                            evdev.ecodes.KEY[event.code]
                        ]
                        await invoke_kwin_shortcut(bus, shortcut)
                    else:
                        print(
                            f"Unknown programmable button: {evdev.ecodes.KEY[event.code]}"
                        )


async def listen_programmable_buttons(bus):
    # Listen for programmable button events
    devices = [evdev.InputDevice(path) for path in evdev.list_devices()]
    for device in filter(lambda d: "Consumer Control" in d.name, devices):
        print(device.path, device.name, device.phys)
        asyncio.ensure_future(listen_programmable_buttons_device(bus, device))


script_arg_mapping = {
    "listen": listen_programmable_buttons,
    "p": "Window to Previous Screen",
    "n": "Window to Next Screen",
    "m": "Window Maximize",
    "l": "Window Quick Tile Left",
    "r": "Window Quick Tile Right",
    "t": "Window Quick Tile Top",
    "b": "Window Quick Tile Bottom",
    "tl": "Window Quick Tile Top Left",
    "tr": "Window Quick Tile Top Right",
    "bl": "Window Quick Tile Bottom Left",
    "br": "Window Quick Tile Bottom Right",
}


async def main():
    bus = await MessageBus().connect()
    if len(sys.argv) < 2:
        print("Usage: window-movement.py <shortcut>")
        return

    shortcut = sys.argv[1]

    if shortcut not in script_arg_mapping:
        print(f"Invalid shortcut: {shortcut}")
        return

    shortcut = script_arg_mapping[shortcut]
    if callable(shortcut):
        await shortcut(bus)
    else:
        print(f"Invoking shortcut: {shortcut}")
        await invoke_kwin_shortcut(bus, shortcut)
        loop = asyncio.get_event_loop()
        loop.stop()


loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)
asyncio.ensure_future(main())
loop.run_forever()
loop.close()
