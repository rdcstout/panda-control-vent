# Local runtime Wi-Fi recovery repair

## Scope and evidence

A powered Vent became unreachable from both Mac and Ubuntu after an overnight print. Network discovery found only the separate Panda Status. Owner power-cycle restored the same address. The RAM console/event log now contains only the new boot; the precise original cause is not established. No claim is made that this outage was reproduced on the physical radio.

Source inspection and a compiled actual-function regression confirm two recovery defects: after a prior successful connection, disconnect events exhausted the startup-only retry limit even though its waiter had returned; the state also remained CONNECTED, disabling the existing no-IP watchdog check. The correction reuses the same handlers, preserves the first-boot retry/fallback policy, restores CONNECTING state, clears stale event bits and keeps reconnecting after a runtime outage. No second recovery loop or new timer is added.

## Verification

- New test fails against the prior handler and passes with the repair. Actual handler functions compiled with strict warnings and ASan/UBSan; scenarios cover initial retry limit, runtime loss beyond that limit, associated/no-IP timeout, eventual IP success, stale timeout after success and AP mode isolation.
- All eleven host tests pass. Clean classic ESP32 ESP-IDF 5.3.1 build and settings/partition upgrade contract pass.
- Targeted adversarial review covered event-loop serialization, boot versus runtime lifecycle, no-IP timing, stale event flags, retry termination, AP isolation, settings preservation and the absence of duplicate recovery mechanisms.
- Exact local OTA: version 0.1.7, project dragonvent, 1,177,040 bytes; SHA-256 3a6a364d8b915b146ffea14bd29e263059f305b1a74488bdbd0838cf8bb107b5. Hardware accepted matching hash/version/length and restarted with software reset reason 3.
- Existing settings, lighting and filament configuration match exact pre-update JSON. Calibration: group 0 open754/closed1490mV; group 1 open755/closed1483mV. Live Bambu subscribed, printer idle, Auto CLOSED at bed24C, motors stopped.
- Candidate BIN/ELF/map/build log retained in sibling panda-control-vent-0.1.7-candidate. Private snapshots and host output remain outside Git in /tmp/pcv-overnight-recovery.

## Delivery state

Owner authorized completing public delivery after the local repair. The tested
0.1.7 BIN is staged for the public release, with a byte-identical permanent-name
copy and checksums. Publish after source CI succeeds, then verify the actual public
downloads and record the release receipt here. No upstream PR or website edit is
included. Keep the known-good public 0.1.6 image for recovery. Current build and
evidence are retained through release verification.
