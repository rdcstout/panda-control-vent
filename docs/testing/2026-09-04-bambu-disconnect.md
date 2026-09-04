# Bambu disconnect validation — 2026-09-04

## Scope

Panda Control Vent 0.1.1 → 0.1.2 on a retail classic ESP32 Panda Vent.
No printer power changes, heating commands, printing jobs, or Moonraker changes.
Physical LED output was not visually observed.

## Hardware procedure and results

1. Confirmed the real printer was connected and idle with bed target zero.
2. Ran a temporary TLS/MQTT broker on the local computer. Kept vents manually
   closed, temporarily redirected only the vent's Bambu client to the broker,
   and preserved the saved printer access code without reading or logging it.
3. On 0.1.1, supplied PAUSE telemetry, closed the broker socket, and observed
   disconnected + paused for ten seconds. Fresh RUNNING telemetry restored
   printing. Repeated with an error report: disconnected + error was retained.
4. Restored the real printer and Auto mode, and compared binding, lighting,
   automation, and provisioning settings.
5. Uploaded the locally built 0.1.2 OTA image. Device-reported upload SHA-256:
   `bd709817489ab6fcd5ac41e3c35a391c3e5e9eeaed24dbb56874cb904576dc3c`.
   Confirmed the running version and settings preservation after restart.
6. Repeated the same broker test: both paused and error became unknown when
   disconnected. Fresh RUNNING reports restored printing. Vents stayed closed.
7. Restored the original printer binding, Auto mode, lighting and automation;
   confirmed the real X1C connected and idle. Temporary broker was stopped.

This exercises an actual MQTT connection loss on the board, not just a settings
reset or power-cycle. It does not measure how often network loss occurs in use.

## Host and build checks

- Production MQTT event handler, report parser, status getter, and completion
  expiry execute in a host harness with ESP32 platform calls stubbed.
- All print phases are checked online/offline, including stale error/printing
  flags; the actual lighting color selector chooses the idle color for NONE.
- Stored state is not mutated by the shared presentation helper.
- Fresh-report recovery works and COMPLETE still expires at 7.5 seconds.
- Tests compile with warnings-as-errors and address/undefined sanitizers.
- Classic ESP32 / ESP-IDF 5.3.1 build passed; application fits the unchanged
  stock-compatible OTA partition with 42% free.
- Upgrade-contract check passed; NVS, partition layout, Moonraker code, motor
  control decisions, and existing lighting configuration were unchanged.
