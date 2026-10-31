# Live printer-source rebinding repair

Scope: Panda Control Vent 0.1.6; the published 0.1.5 baseline is
`5d7be2d829b8680994fb2e07d799cfea50352288`.

## Confirmed defects and changes

The controller booted on Klipper and was bound to Bambu without restarting.
The API read the selected Bambu source while automatic policy used its boot-time
Klipper copy. With Auto, subscribed Bambu, PETG and bed/target 70 C, the vent stayed
closed instead of following the 85 C seal threshold. The policy now reads the
current source exactly once per tick. No temperature decision or NVS format changes.

Hardware acceptance exposed two companion defects on the same update/rebinding
path. Motor calibration loaded before NVS was initialized, losing its saved
endpoints on every boot. A single shared, success-cached storage initializer now
runs before motor initialization and is reused by Wi-Fi startup; existing storage
recovery conditions are unchanged. ADC/motor initialization still precedes all
Wi-Fi/MQTT workers. No NVS erase was performed during this repair.

Selecting Klipper after a Bambu boot also asserted on an uninitialized Moonraker
mutex, confirmed by reset_reason 4 (panic). The local Moonraker component override
returns its initial disabled snapshot and invalid-state error before startup;
initialized behavior is unchanged. Its VENDORING.md identifies the exact licensed
upstream source and six-line runtime delta. Reset reason is now recorded in the
boot event to distinguish software restarts from faults.

## Automated verification and review

- Ten host tests pass, including compiled production functions under ASan/UBSan
  and strict compiler warnings. All three new regressions failed before correction:
  wrong OPEN target after live Bambu binding, calibration not loaded at startup,
  and null-semaphore assertion from unstarted Moonraker status.
- Coverage includes both source directions, NONE, unavailable/connecting clients,
  reconnect hold, manual/Auto, exact threshold/cooldown boundaries, Advanced
  material rules, Klipper error hold, paired heater interlock, and both motor groups.
- Separate Bambu parser ASan/UBSan executable and settings-upgrade contract pass.
- Clean classic ESP32 ESP-IDF 5.3.1 build passes with no compiler warnings. All
  three managed pins match the lock; the build selects the local Moonraker override.
  Managed manifest/lock changes remove only the overridden component; no dependency
  version upgrade or absolute path was introduced.
- Adversarial review covered source selection, startup ordering, storage failure/
  retry behavior, settings compatibility, client initialization and disconnects.
  Independent review confirmed the startup/getter corrections and dependency delta.
  Existing motor, parser, interface and release behavior remain on the prior baseline.

## Exact artifact and hardware acceptance

Final image: `Panda-Control-Vent-0.1.6-OTA.bin`, 1,176,880 bytes.
SHA-256: `f591c86cd9bd2a93b25997e1a21cb431b3526b01fd8f3dadb0b6eb3cccc9b671`.
Classic ESP32 image checksum/hash, project dragonvent and version 0.1.6 validated
with esptool. The permanent filename contains identical bytes.

Roger explicitly authorized updating the standalone Vent during the active print.
No printer commands were sent. Settings snapshots were retained privately outside
Git. The exact final image was installed through the established OTA API; the
response matched its version, byte count and SHA-256. Boot reason 3 confirmed the
expected software restart. Firmware 0.1.6 returned with the existing Bambu binding,
Wi-Fi, thresholds, lighting and filament rules retained. Saved calibration loaded
exactly: group 0 OPEN 754/CLOSED 1490mV; group 1 OPEN 755/CLOSED 1483mV.

Repeated actual-device regression:

1. Manual CLOSE: both motor groups reached their closed endpoints.
2. NONE + Auto: target held CLOSED.
3. Klipper selected without starting its client: target held CLOSED, no panic.
4. Bambu selected without reboot: fresh subscribed PETG 70 C telemetry drove Auto
   OPEN; both motors reported arrival. One run measured group 0 at 752mV/300ms and
   group 1 at 755mV/320ms. A repeat measured both arrivals within 320ms.
5. Restored Bambu + Auto OPEN, original 85/45/35 C thresholds, lighting/binding and
   calibration. Two complete source-switch sequences and a further 30-second
   observation retained the same software-reset boot event and stable operation.

The initial Manual OPEN state was a workaround; final Auto mode restores the
intended repaired operation. The printer continued its PETG job throughout.
A real Klipper printer and paired heater are unavailable; their policy behavior
is verified by host regression, not new physical printer/heater testing.

## Delivery checkpoint

Implementation, automated tests, independent review, clean build and exact-byte
hardware acceptance are complete. Own-repository source/release delivery and
post-publication download verification are next. No upstream PR or website change
is included. Preserve the existing 0.1.5 versioned image for recovery.

Build/evidence: `/tmp/panda-control-vent-016-final-build` and matching `.log`;
private validation snapshots: `/tmp/pcv-016-private-validation` (not for upload).
Durable candidate: sibling `panda-control-vent-0.1.6-candidate` directory.
Temporary build cleanup will be proposed only after delivery; source, release
artifacts and verification evidence remain retained.
