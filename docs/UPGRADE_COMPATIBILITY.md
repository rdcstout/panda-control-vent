# DragonVent 0.5.9 upgrade compatibility

Panda Control Vent is designed as an in-place OTA upgrade from DragonVent
0.5.9. Uploading the Panda Control Vent OTA image writes only the inactive app
partition. It does not erase or replace the NVS settings partition.

## Preserved settings

The fork retains DragonVent 0.5.9's `app_nvs` namespace and persistence keys
for:

- normal Wi-Fi name and password;
- Bambu printer IP address, serial number, and LAN access code;
- selected printer/control source;
- vent policy mode, manual target, temperature thresholds, and filament rules;
- vent motor hall-sensor calibration;
- lighting colors, brightness, effects, state behavior, and strip direction;
- Moonraker settings and the optional local control token.

The existing lighting blob is a prefix of the expanded lighting structure.
During the first boot, all existing bytes are loaded over initialized defaults.
The two new chamber-light fields are appended, so an older lighting blob keeps
its original vent-light values and starts with chamber lighting linked to the
vent lights. Nothing is rewritten until the user saves lighting settings.

## Intentional migration

The setup access point is the only deliberately changed stored behavior. An
inherited `Always on` or timed setup-AP mode is converted to automatic fallback:
the AP remains off whenever normal Wi-Fi connects and appears only if that join
fails. An explicitly disabled recovery AP remains disabled.

## Release gate

Run `python3 scripts/check_upgrade_contract.py` before producing an OTA image.
It verifies the unchanged partition table, pinned shared-component baseline,
stable NVS keys, append-only lighting layout, and app-partition-only OTA path.
