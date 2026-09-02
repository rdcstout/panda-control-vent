# Panda Control Vent Firmware

ESP-IDF 5.3.1 project targeting the classic ESP32 in the BIGTREETECH Panda
Vent. The internal ESP-IDF project name remains `dragonvent` for OTA and
upgrade compatibility; the product, hostname, mDNS identity, and interface are
Panda Control Vent.

## Reproducible build

From the repository root:

```sh
node --test tests/*.test.mjs
cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
  -Ifirmware/components/dc_bambu/include \
  tests/dc_bambu_light_parse_test.c -o /tmp/pcv-bambu-test
/tmp/pcv-bambu-test
python3 scripts/check_upgrade_contract.py
tools/idf-build.sh firmware esp32 build
```

The build wrapper requires ESP-IDF 5.3.1, checks the target compiler, and
verifies the committed dependency lock against immutable dragon-core object
IDs. CI uses the same wrapper.

## Release image

`firmware/build/dragonvent.bin` is the application-only OTA image. It is used
for compatible stock installations and in-place DragonVent/Panda Control Vent
updates. The existing bootloader and partition table are retained.

The GitHub release workflow publishes:

- `Panda-Control-Vent-<version>-OTA.bin`;
- `Panda-Control-Vent-OTA.bin`; and
- `SHA256SUMS`.

## Layout

```text
firmware/
├── CMakeLists.txt
├── partitions.csv
├── sdkconfig.defaults
├── dependencies.lock
├── main/
│   ├── app_main.c
│   └── idf_component.yml
└── components/
    ├── dc_bambu/     Bambu discovery, MQTT telemetry, and state mapping
    ├── dc_ui/        embedded Panda Control Vent SPA
    ├── dc_wifi/      station, setup AP, migration, and recovery state
    ├── dv_motor/     PWM drive and hall-sensor state machine
    ├── dv_policy/    automatic/manual vent policy
    ├── dv_portal/    API v2 and product setup adapter
    └── dv_rgb/       vent and chamber-light zone rendering
```

Shared DragonVent components remain pinned from dragon-core. Product-specific
components and the complete embedded interface live in this repository.

## Persistence and upgrades

The OTA/NVS partition layout and DragonVent 0.5.9 key names are release
contracts. New lighting fields are appended after the compatible DragonVent
prefix so existing settings remain readable. Run
`scripts/check_upgrade_contract.py` before every build or release.

## Hardware status

Version 0.1.0 has been hardware-tested with a Bambu Lab X1C. Klipper/Moonraker support
is inherited from DragonVent and has not been independently tested by
Extrusion Therapy.
