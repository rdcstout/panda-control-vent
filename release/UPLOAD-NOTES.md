# Panda Control Vent 0.1.0

## Files

- `Panda-Control-Vent-0.1.0-OTA.bin` — versioned release image.
- `Panda-Control-Vent-OTA.bin` — identical image using the stable download
  filename.
- `SHA256SUMS` — checksums for both filenames.

Both BIN files are byte-identical final builds of the firmware that completed
extended hardware testing on a Panda Vent connected to a Bambu Lab X1C.

## Upload

From compatible stock firmware, use the existing stock firmware-update page.
From DragonVent or Panda Control Vent, open **Setup → Maintenance → Firmware
update**. Upload either BIN and wait for the interface to confirm that the
controller has restarted.

Expected displayed version after restart: `0.1.0`.

## Verified behavior

- Bambu discovery, binding, reconnect, and telemetry.
- Automatic and manual vent control across complete prints.
- Preparing, printing, pause/resume, completed, stopped, and idle status
  mapping on hardware.
- Bambu error parsing and status mapping in host-side tests. A genuine printer
  fault was not induced during hardware testing.
- Completed color for 7.5 seconds, then idle.
- Optional idle dimming to 20% after a three-second transition delay.
- Independent pixels 0–10 for vent lighting and 11–15 for chamber lighting on
  both GPIO outputs.
- Factory chamber-light following.
- OTA settings retention.

## Known limitations

- Factory reset retains some lighting, calibration, access-point, and local
  control settings.
- Klipper/Moonraker support is inherited from DragonVent and has not been
  independently hardware-tested by Extrusion Therapy.

Panda Control Vent is unofficial community firmware. Keep official Panda Vent
stock firmware available before installing.
