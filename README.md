# X-IoT Gateway

Firmware for an ESP32-S3 industrial gateway that polls **Modbus RTU** devices over RS485,
publishes tag values to an **MQTT** broker, accepts write commands over MQTT, and is
configured through a built-in **web portal**.

```
RS485 Modbus RTU slaves
   ↑ read / ↓ write
modbus_service ← protocol_dispatcher ← acquisition   (periodic scan of all readable tags)
       │                              ← subscriber    (MQTT command → Modbus write)
       ↓
  tag_runtime (latest value of every tag)
       ├→ publisher → mqtt_service → broker
       └→ web_server (REST API) ← portal (single-file SPA served from LittleFS)
```

## Hardware

| Item | Value |
|---|---|
| MCU board | Waveshare ESP32-S3R8 (16 MB flash, 8 MB octal PSRAM) |
| Modbus RS485 | `Serial1`, RX/TX pins from config (default RX 18, TX 17), DE/RE on GPIO 21 |
| Setup hotspot | SSID `Xpredict_Gateway`, portal at http://192.168.4.1 |

## Repository layout

| Path | Contents |
|---|---|
| `src/`, `include/` | Firmware modules (one `.cpp`/`.h` pair per service) |
| `data/` | LittleFS image: `system_config.json`, `tag_config.json`, built `index.html` |
| `portal/` | Web portal sources (Vite, vanilla JS). Builds into `data/index.html` |
| `test/` | Host-side unit tests (`pio test -e native`) |
| `.github/workflows/ci.yml` | CI: unit tests, firmware + LittleFS build, portal build check |

## Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) (or the VS Code extension)
- Node.js 20+ (only to change the web portal)
- A host C/C++ compiler (gcc/g++, or MinGW on Windows) for unit tests

## Build and flash

```bash
pio run                      # build firmware
pio run -t upload            # flash firmware over USB
pio run -t uploadfs          # flash LittleFS (config files + portal)
pio device monitor           # serial log at 115200 baud
```

Flashing LittleFS **overwrites the device configuration** with the files in `data/`.

## Web portal

```bash
cd portal
npm ci
npm run dev       # local dev server (API calls need a device or a mock)
npm run build     # writes ../data/index.html
```

Commit the rebuilt `data/index.html` together with portal source changes; CI fails if it is stale.

## Unit tests

```bash
pio test -e native
```

Only hardware-independent code is compiled for the `native` environment (see
`build_src_filter` in `platformio.ini`). Keep such code free of Arduino/ESP-IDF headers,
for example by depending on `include/tag_types.h` rather than `tag_registry.h`.

## Configuration

`data/system_config.json` holds Wi-Fi, MQTT, Modbus serial, scan/publish intervals
and portal credentials. `data/tag_config.json` is the tag list. Each tag looks like:

```json
{
  "tagId": 2,
  "name": "MODE-YT",
  "unit": "Status",
  "topic": "XP/TW-01/MODE-YT-100",
  "topicType": "PUBLISH",
  "source": "MODBUS",
  "access": "READ_ONLY",
  "dataType": "TAG_BOOL",
  "modbus": { "slaveId": 2, "address": 12, "quantity": 1, "functionCode": 1, "wordOrder": "WORD_CDAB" }
}
```

Supported values: `topicType` PUBLISH | SUBSCRIBE; `access` READ_ONLY | WRITE_ONLY | READ_WRITE;
`dataType` TAG_BOOL | TAG_UINT16 | TAG_INT16 | TAG_UINT32 | TAG_INT32 | TAG_FLOAT32 | TAG_STRING;
`wordOrder` WORD_ABCD | WORD_CDAB (BADC/DCBA are parsed but not yet decoded correctly).

## Dependencies

All versions are pinned in `platformio.ini` (platform, git tag or commit for each library).
Upgrade them deliberately in a dedicated commit and let CI confirm the build.
