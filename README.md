# Panda Control Vent

Panda Control Vent is an unofficial community firmware for the BIGTREETECH
Panda Vent. It is developed by Extrusion Therapy as the embedded companion to
the Panda Control desktop app.

The project is currently in active hardware-development. No public installation
image is released yet.

## Goals

- Preserve DragonVent's proven motor, hall-sensor, vent-policy, networking,
  recovery, and OTA behavior.
- Make Bambu LAN setup validate immediately and reconnect without a device
  reboot.
- Treat the relocated front LED boards as a separately assignable chamber-light
  zone with the same colors, brightness, effects, speed, and printer-state
  behavior as the main vent lights.
- Provide a complete embedded setup, maintenance, recovery, and operational UI
  using the restrained Panda Control visual language.
- Preserve an existing DragonVent 0.5.9 installation's Wi-Fi, printer binding,
  policy, calibration, and lighting settings during an OTA upgrade.
- Keep the setup access point off after normal Wi-Fi connects and bring it back
  only as an optional connection-failure fallback.
- Keep Panda Control desktop focused on everyday operation of devices that are
  already configured.

## Project lineage

The fork starts from [DragonVent](https://github.com/justinh-rahb/DragonVent)
v0.5.9 at commit `51d1c1ea09e0f752b030de56f7b7a9b42fda6518` and uses
the Dragon Core v0.30.0 components. The original MIT copyright and license are
preserved in [LICENSE](LICENSE).

The upstream repository is retained locally as the read-only `upstream` remote.
Our public repository will be configured only after the firmware has passed
hardware testing.

## Product boundaries

See [Product Contract](docs/PRODUCT_CONTRACT.md) for the embedded-versus-desktop
boundary and [LED Hardware Map](docs/LED_HARDWARE_MAP.md) for the mapping proven
on retail hardware. The in-place persistence contract is documented in
[Upgrade Compatibility](docs/UPGRADE_COMPATIBILITY.md).

## Development safety

- Start every firmware release from a named, reproducible upstream baseline.
- Keep the last known-good OTA image available before hardware testing.
- Never replace normal startup services with diagnostic code.
- Experimental controls must be additive, bounded, and recoverable by reboot.
- Do not publish or submit upstream changes until the fork passes device and
  Panda Control integration testing.

## Status

Version `0.1.0-rc.2` contains the complete embedded interface, immediate Bambu
reconnection flow, automatic setup-AP fallback, and independent vent/chamber
lighting implementation. Build and static verification are complete; installation
and end-to-end hardware validation remain release-candidate tests.
