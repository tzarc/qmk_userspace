#!/usr/bin/env -S uv run --script
# Copyright 2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "dbus-next",
# ]
# ///

from dbus_next.aio import MessageBus

import asyncio


async def main() -> None:
    bus = await MessageBus().connect()
    introspection = await bus.introspect(
        "org.freedesktop.PowerManagement", "/org/freedesktop/PowerManagement/Inhibit"
    )
    obj = bus.get_proxy_object(
        "org.freedesktop.PowerManagement",
        "/org/freedesktop/PowerManagement/Inhibit",
        introspection,
    )

    # Connect to the Inhibit interface and register a signal handler
    iface_inhibit = obj.get_interface("org.freedesktop.PowerManagement.Inhibit")

    def on_inhibit_changed(arg) -> None:
        pass

    iface_inhibit.on_has_inhibit_changed(on_inhibit_changed)

    # Check current state
    is_inhibited = await iface_inhibit.call_has_inhibit()
    print(f"Inhibit state: {is_inhibited}")

    # Connect to the Properties interface and register a signal handler
    iface_props = obj.get_interface("org.freedesktop.DBus.Properties")

    def on_properties_changed(
        interface_name: str, changed_properties, invalidated_properties
    ) -> None:
        for changed, variant in changed_properties.items():
            print(f"property changed: {changed} - {variant.value}")

    iface_props.on_properties_changed(on_properties_changed)

    await bus.wait_for_disconnect()


loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)
asyncio.ensure_future(main())
loop.run_forever()
loop.close()
