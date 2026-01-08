# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XAP Binding is a TypeScript library that generates RPC-style bindings from QMK XAP protocol specifications. It translates user-facing API calls into byte buffers for USB HID communication. Transport is injected by the consuming environment (WebHID, node-hid, Tauri).

**Status:** Specification complete, implementation not yet started.

## Key Documentation

- **README.md** — Quick start and overview
- **SPEC.md** — Complete technical specification (architecture, API design, protocol details)
- **versions/** — XAP protocol spec JSON files (0.0.1 through 0.3.0)

## Architecture (from SPEC.md)

```
src/
├── index.ts              # Public exports
├── client.ts             # createXapClient factory
├── types.ts              # TypeScript interfaces
├── constants.ts          # SecureStatus (exported), Token (internal)
├── errors.ts             # XapError, XapSecureError, XapTimeoutError, XapUnsupportedError
├── emitter.ts            # Minimal event emitter (zero dependencies)
├── core/
│   ├── buffer.ts         # Binary encoding/decoding (little-endian)
│   ├── token.ts          # Token generation and tracking
│   ├── codec.ts          # Type system codec (u8, u16, struct, etc.)
│   └── protocol.ts       # Packet construction/parsing
└── generator/
    └── api.ts            # Generates nested namespace from spec
```

## Design Decisions

- **ESM only** — No CommonJS
- **Zero dependencies** — Custom minimal event emitter
- **Transport agnostic** — Consumer injects `{ write, onData, close? }` interface
- **Runtime spec loading** — Specs passed to factory, can be fetched dynamically
- **Capability checking** — Library queries device capabilities, throws `XapUnsupportedError` for unsupported commands
- **Nested API** — `xap.qmk.boardIdentifiers()` mirrors spec hierarchy

## Protocol Notes

- All integers are little-endian
- Tokens: `0x0100`–`0xFFFD` (valid), `0xFFFE` (fire-and-forget), `0xFFFF` (broadcast)
- Struct member names convert from "Vendor ID" to `vendorId` (camelCase)
- XAP versions are backwards/forwards compatible — capabilities query is source of truth
