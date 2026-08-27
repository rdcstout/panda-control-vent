# Product Contract

## Embedded firmware interface

The embedded Panda Control Vent interface is the complete device surface. It
owns:

- vent status, automatic/manual operation, and operational thresholds;
- vent-light and chamber-light controls;
- Wi-Fi station and recovery access-point configuration;
- Bambu LAN discovery, credentials, validation, and connection diagnostics;
- firmware update, reboot, logs, hardware diagnostics, and factory reset.

Setup actions must be clearly separated from everyday controls, but they remain
available on the device for recovery when no desktop app is present.

## Panda Control desktop interface

Panda Control manages devices only after factory or embedded setup has placed
them on the local network. It owns:

- discovery of already-configured devices and manual IP/hostname entry;
- status and telemetry;
- normal vent operation and thresholds;
- vent-light and chamber-light operation.

It must not expose Wi-Fi/AP provisioning, printer credentials, source binding,
OTA updates, hardware mapping, factory reset, or other setup/recovery controls.
Printer connectivity may be shown read-only.

## Shared API contract

The embedded UI and Panda Control use one versioned JSON API. The firmware is
the source of truth. Existing DragonVent v2 endpoints remain compatible while
new capabilities are additive.

Lighting compatibility rules:

1. Existing `/api/v2/lighting` fields continue to represent the primary vent
   lighting configuration.
2. A new zone object exposes `linked`, `vent`, and `chamber` configuration.
3. Linked mode is the migration/default behavior and makes chamber lighting
   follow the vent configuration exactly.
4. Independent mode gives the chamber zone its own full lighting configuration.
5. Older clients that never send zone fields continue to work.

## Visual direction

- Dark mode by default with an optional light appearance.
- Layered graphite surfaces, lighter inset controls, subtle elevation, and
  restrained red accents.
- Top navigation, native-feeling controls, compact conventional buttons, and
  concise labels.
- Responsive reflow for the embedded browser interface; no fixed desktop-only
  scaling behavior.
- Attribution: `An Extrusion Therapy workshop tool` with a link to
  `https://extrusiontherapy.com/`.
