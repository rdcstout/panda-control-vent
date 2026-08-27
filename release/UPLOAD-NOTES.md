# Panda Control Vent 0.1.0-rc.1

This is an OTA application image for an installed DragonVent 0.5.9 Panda Vent.
Upload `Panda-Control-Vent-0.1.0-rc.1-ota.bin` from the embedded firmware upload
control. Do not use the file as a full USB factory image.

The OTA process writes the inactive application partition and preserves the NVS
settings partition. Existing Wi-Fi, Bambu printer binding, policy, calibration,
and lighting settings should remain available after reboot. The setup AP is
intentionally migrated to automatic fallback behavior.

Keep the known-good DragonVent 0.5.9 OTA BIN and the stock BQ BIN available
during release-candidate testing. If the web interface remains reachable, either
can be uploaded through the firmware maintenance control to roll back.

After upload, verify the displayed firmware version is `0.1.0-rc.1`, the Bambu
connection reaches `Connected and receiving live data`, existing settings are
unchanged, and the Vent Lights and Chamber Lights can be saved independently.
