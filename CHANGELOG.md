# Changelog

All notable changes to the **Panda Control Vent** firmware are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/); versions are the
firmware release tags (`vX.Y.Z`). The release workflow pulls the matching section
below into the GitHub Release notes.

## [Unreleased]

## [0.1.2] - 2026-09-04

### Fixed
- Stop presenting cached Bambu print phases as live after a connection loss.
  The API reports unknown while disconnected; printer-status lighting uses
  the configured idle appearance until fresh printer reports resume.
- Use the same connection-aware state mapping for lighting, the API, and
  Bambu diagnostic logs.

### Validation
- Reproduced stale paused/error states on 0.1.1 using a local simulated Bambu
  broker with a real Panda Vent ESP32; repeated the sequence on 0.1.2 and
  verified unknown states on disconnect and correct fresh states on reconnect.
- Verified OTA settings retention and restored the original X1C binding,
  Auto mode, lighting, and automation settings after testing.
- Added compiled host regressions for all phases, actual MQTT event handling,
  idle color selection, and the unchanged 7.5-second completion timer.

## [0.1.1] - 2026-09-02

### Fixed
- Harden Bambu delta-state handling so a stale printer error cannot mask a new
  active print when the next report omits `print_error`.
- Parse `print_error` only from the live `print` object and reject malformed or
  out-of-range values instead of accepting unrelated or truncated data.
- Use one authoritative completion-expiry rule for both incoming reports and
  status polling while preserving the 7.5-second Completed indication.
- Reset all Bambu phase, error, and completion tracking state when the client is
  reset.
- Serialize Bambu MQTT lifecycle and configuration changes so live reconnects
  cannot race with start, stop, or configuration clearing.
- Preserve the active connection when clearing stored configuration fails.

## [0.1.0] - 2026-09-01

### Added
- Optional **Dim while idle** control for the independent chamber-light zone;
  idle brightness is reduced to 20% without affecting printing brightness.
- Optional **Follow printer chamber light** control. On Bambu printers, the
  chamber-light zone follows the factory light's reported MQTT on/off state.

### Changed
- Give Vent State and Printer Status lighting modes distinct, unambiguous color
  controls; the legacy printing-color override is retired.
- Correct the lighting map text to match the hardware-proven zero-based ranges:
  vent LEDs 0-10 and chamber LEDs 11-15.
- Rebrand the hostname, mDNS service, and generated setup/recovery AP from
  DragonVent to Panda Control Vent while preserving custom AP names.
- Show the active printing and cooldown policies directly beneath the Overview
  summary, including the configured automatic closing temperature.
- When **Dim while idle** is enabled, keep the Idle color at full brightness for
  three seconds after any job-ending transition before reducing it to 20%.
  Startup already in Idle still dims immediately; pause/resume is unaffected.

### Fixed
- Preserve Bambu's detailed print phase through the status and RGB pipelines so
  manual or automatic pauses use the configured Paused color instead of the
  Printing color. Preparing, Completed, and Error now retain their distinct
  configured colors as well.
- Match Spooly's proven Bambu stop/error handling: `FAILED` with a zero
  `print_error` is a user-stopped/idle state, while a nonzero error code uses the
  configured Error color.
- Match Spooly's one-shot Bambu completion behavior: show the Completed color
  for 7.5 seconds after an observed active job, then return to Idle even when an
  X1 printer leaves `gcode_state` at `FINISH`. A stale `FINISH` seen at startup
  no longer produces a false completion state.

## [0.5.9] - 2026-08-26

### Changed
- Pin **dragon-core v0.30.0** (from v0.25.0) — headline is **Wi-Fi join
  reliability**: disables WiFi modem power-save (fixes "associated but never gets
  a DHCP IP"), a no-DHCP-IP watchdog that skips a mesh node that admits but won't
  lease, more connect retries, and an opt-in **fallback** AP mode. Also carries
  the intervening core work (Prusa/PrusaLink source, Bambu chamber-follow +
  freshness, serial-redaction, shared UI updates). Build-verified; the Wi-Fi
  fixes were hardware-proven on a C3 (DragonWheeze) in a multi-AP mesh.

### Fixed
- Handle the new `DC_SRC_PRUSA` control-source enum in the source switch (Vent
  has no Prusa support, so it logs and no-ops like the other unsupported sources).

## [0.5.8] - 2026-08-19

### Added
- **Seal the vent while the chamber heater is heating.** In AUTO, DragonVent now
  closes the vent whenever a paired **DragonBreath** (via the `dragonbreath-klipper`
  helper) is deliberately heating — `connected && !fault && !inhibited &&
  device_target > 0 && mode ∈ {power_on, auto}`. This covers a chamber heat soak,
  where a warm bed at idle would otherwise open the vent to shed residual heat.
  Uses the helper's confirmed state over Moonraker (re-pins **dragon-core v0.25.0**);
  no extra configuration. Turning the chamber heater off returns to the normal
  bed/material policy. Hardware-validated end-to-end.

## [0.5.7] - 2026-08-19

### Fixed
- **Moonraker connection can no longer silently die.** A half-open WebSocket used to
  leave the vent "connected" but frozen on stale printer data. Re-pins **dragon-core
  to v0.24.0**, which adds WebSocket ping/pong plus a staleness watchdog that
  reconnects and re-subscribes if no update arrives for ~45 s. Hardware-validated
  across a printer-host reboot.

## [0.5.6] - 2026-08-18

### Changed
- **Adopt the shared `dc_lighting` engine** (dragon-core), ending the duplicate LED
  effect code between DragonVent and DragonStatus. `dv_rgb` is now a thin adapter that
  keeps the vent's color policy and delegates rendering to the shared engine. Per-strip
  reverse and the state-colored Cylon effect are preserved; SPI+DMA transport and the
  per-strip in-sync layout are unchanged.

## [0.5.5] - 2026-08-14

### Added
- **Per-strip LED reverse.** Each WS2812 strip's direction is independent (Reverse
  strip 1 / 2), so a vent whose strips are fed from opposite connectors can run them
  the same way or opposite — letting effects "circle" the printer.
- **Cylon eye** as a distinct effect (previously it fell through to Marquee).

## [0.5.4] - 2026-08-11

### Fixed
- Enable the esp-tls options the Bambu LAN client requires so **Bambu LAN can connect**
  (re-pins dragon-core to v0.14.0).

## [0.5.3] - 2026-08-11

### Fixed
- Clean re-cut of 0.5.2: the build now stamps a clean version and `dependencies.lock`
  is untracked (fixes the "-dirty" release version).

## [0.5.2] - 2026-08-11

### Added
- **On-demand Bambu LAN discovery** in setup (SSDP scan + printer picker).

### Fixed
- **Rainbow flicker** on the classic ESP32 — drive WS2812 over SPI+DMA instead of RMT,
  whose refill ISR could be starved mid-frame.

## [0.5.1] - 2026-08-11

### Added
- **RGB status lighting** — WS2812 effects (Strobe/Wave/Marquee, …), reverse LED
  direction, and per-printer-status colors (Follow Printer mode).

### Changed
- **OTA-only install** — dropped the `factory.bin` download and de-referenced the USB
  helper from the release.

## [0.5.0] - 2026-08-11

### Added
- **Install over stock, no USB.** DragonVent runs on the stock Panda Vent partition
  table, so it installs and updates entirely from the web UI through the stock
  firmware's own OTA — the stock bootloader is preserved and you can revert to stock.
