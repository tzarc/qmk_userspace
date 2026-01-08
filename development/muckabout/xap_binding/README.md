# XAP Binding

A TypeScript library that generates RPC-style bindings from QMK XAP protocol specifications.

## What It Does

- Parses XAP specification JSON files
- Generates a nested API matching the protocol hierarchy
- Encodes requests and decodes responses (little-endian binary)
- Handles token correlation for concurrent requests
- Emits events for broadcast messages
- Checks device capabilities and throws typed errors for unsupported commands
- Zero runtime dependencies

## What It Doesn't Do

- **No USB HID transport** — You provide a transport interface for your environment (WebHID, node-hid, Tauri, etc.)

## Quick Example

```typescript
import { createXapClient } from 'xap-binding';
import spec from 'xap-binding/versions/xap_0.3.0.json';

// You provide the transport
const transport = createWebHIDTransport(device);

// Create client
const xap = createXapClient(spec, transport);

// Use the API
const version = await xap.xap.versionQuery();
console.log(`XAP ${version.major}.${version.minor}.${version.patch}`);

const board = await xap.qmk.boardIdentifiers();
console.log(`${board.vendorId}:${board.productId}`);

// Listen for broadcasts
xap.on('log', (msg) => console.log(msg));
```

## Transport Interface

```typescript
interface XapTransport {
  write(buffer: Uint8Array): Promise<void>;
  onData(handler: (buffer: Uint8Array) => void): void;
  close?(): Promise<void>;
}
```

## Specification

See [SPEC.md](./SPEC.md) for complete technical specification including:

- Protocol details and packet formats
- Type codec mappings
- API design and method signatures
- Error handling
- Secure route flow
- Specification composition for keyboard/user extensions

## Supported Environments

- Browser (via WebHID)
- Electron
- Tauri
- Node.js CLI (via node-hid)

## License

TBD
