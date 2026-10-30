# Factory reset credential migration regression

Scope: Panda Control Vent 0.1.5.

## Reproduction

The retail Vent running 0.1.2 acknowledged its web factory reset, then returned
on its former LAN address with Wi-Fi in station mode and printer source reset
to Klipper. No Vent setup hotspot appeared in macOS Wi-Fi settings. Startup
could repopulate missing current keys from retained stock NVS blobs.

## Correction

The web and BOOT reset paths share product setup cleanup. It removes stock
Wi-Fi/Bambu/Moonraker/HA migration blobs and setup AP overrides/control token,
then clears printer and policy configuration. The callers clear current Wi-Fi
credentials. Lighting and motor calibration are preserved. Wi-Fi key erasure
and commit errors are propagated; the BOOT path does not reboot on failure.
Normal OTA migration is unchanged.

## Verification

- Seven Node tests pass, including compiled real reset helpers under ASan/UBSan.
  Cases cover retained/current credentials, preservation of lighting/calibration,
  repeated resets, missing keys, open/erase/commit failures, and common reset routing.
- Settings-upgrade contract passes; clean ESP-IDF 5.3.1 ESP32 build succeeds.
- Candidate image: `/tmp/panda-control-vent-015-build/dragonvent.bin`.
  Size 1,176,688 bytes; image checksum/hash valid; version 0.1.5.
  SHA-256: `b0ca6ba39ae3a0b7832cc7eb88d74ed35e7834f13118e383d97baa23ea2b5705`.
- Live OTA response matched the candidate version, byte count, and SHA-256.
- Live state confirmed installed 0.1.5 before the corrected web reset.
- Corrected reset acknowledged success; old LAN address stopped responding.
- macOS Wi-Fi settings visibly listed `Panda_Control_Vent_C951` at full signal,
  alongside the previously reset Panda Status hotspot.

The Vent is intentionally left unconfigured for transfer to its buyer. The
BOOT-button path shares the verified cleanup but was not physically pressed.
No source push, release, or reconfiguration of the buyer-ready unit was performed
in this local validation step.

## Publication

Roger subsequently authorized public release. Tag v0.1.5 points to
`63e1744aa15b4a46fbafa6550ac19de3c3766dfe`; its firmware/test source matches
the locally tested candidate. Firmware Build `41131988372` and Firmware Release
`41132259950` passed. The exact hardware-tested BIN was published under both
the versioned and stable filenames. Both release assets and the permanent
latest-release download were retrieved and compared byte-for-byte with the
tested candidate; SHA256SUMS passed with the hash above.
