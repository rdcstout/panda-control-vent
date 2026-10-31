# Panda Control Vent 0.1.6

- `Panda-Control-Vent-0.1.6-OTA.bin`: versioned firmware.
- `Panda-Control-Vent-OTA.bin`: byte-identical permanent download name.
- `SHA256SUMS`: checksums for these two filenames.

Follow the [installation/update guide](../docs/INSTALLATION.md).
Expected displayed firmware after restart: **0.1.6**.

This release makes automatic vent control follow live printer-source changes,
prevents a restart when selecting an unstarted Klipper source, and loads saved
motor calibration correctly at startup. Existing settings are preserved.
See the [changelog](../CHANGELOG.md#016---2026-10-31).
