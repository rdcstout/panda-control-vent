# Panda Control Vent

<p align="center">
  <img src="docs/assets/panda-control-vent-hero.png" alt="Panda Control Vent firmware and embedded interface" width="100%">
</p>

<p align="center">
  Community firmware for the BIGTREETECH Panda Vent, developed and hardware-tested by
  <a href="https://extrusiontherapy.com/">Extrusion Therapy</a>.
</p>

<p align="center">
  <a href="https://github.com/rdcstout/panda-control-vent/releases/latest/download/Panda-Control-Vent-OTA.bin"><strong>Download firmware</strong></a>
  ·
  <a href="https://makerworld.com/en/models/3250883-panda-control-vent#profileId-3684253">Printable model files</a>
  ·
  <a href="https://github.com/rdcstout/panda-control-vent/releases/latest">Release notes</a>
  ·
  <a href="https://buy.stripe.com/fZu3cw2Mnfr0d7N3ws1kA00">Support Future Tools</a>
</p>

> [!IMPORTANT]
> Panda Control Vent is unofficial community firmware. It is not endorsed by
> BIGTREETECH or Bambu Lab. Keep an official stock firmware image available so
> you can return the controller to factory software.

## What it adds

- A complete, responsive embedded interface for control, setup, maintenance,
  recovery, and firmware updates.
- Hardware-tested Bambu LAN support with correct preparing, printing, paused,
  completed, stopped, and idle status behavior. Error parsing and mapping are
  covered by host-side tests; a genuine printer fault was not induced during
  hardware testing.
- Simple commanded-bed-temperature automation for Bambu printers, with the
  inherited advanced material policy retained for Klipper users.
- Independent vent and chamber-light zones. The factory front LED boards can be
  relocated inside the printer and controlled as full-color chamber lighting.
- Separate colors, brightness, effects, speed, error behavior, idle dimming,
  and factory chamber-light following.
- In-place OTA upgrades that preserve compatible DragonVent and Panda Control
  Vent settings.
- Optional setup/recovery access point that remains off while normal Wi-Fi is
  working.

## Tested configuration

The `0.1.0` baseline received extended testing on retail Panda Vent hardware
connected to a Bambu Lab X1C. Real-device testing of that baseline covers:

- Bambu LAN discovery, binding, reconnect, and live printer telemetry;
- automatic and manual vent operation through complete print cycles;
- preparing, printing, manually paused, completed, stopped, and idle colors;
- a 7.5-second completion indication followed by idle;
- optional idle dimming to 20% of the selected brightness after a three-second
  transition delay;
- factory chamber-light following;
- independent right and left vent/chamber LED zones; and
- OTA updates that retain existing settings.

Release `0.1.1` adds Bambu state-handling and reconnect corrections, verified by
automated tests and a successful firmware build. That verification is separate
from the baseline's extended hardware testing. See the
[release notes](https://github.com/rdcstout/panda-control-vent/releases/latest)
for changes in the current download.

Klipper/Moonraker support is inherited from DragonVent. It remains available,
but Extrusion Therapy has not independently hardware-tested that path.

## Download and install

Download the current **[Panda-Control-Vent-OTA.bin](https://github.com/rdcstout/panda-control-vent/releases/latest/download/Panda-Control-Vent-OTA.bin)**. This permanent URL always resolves to the stable OTA image from the latest release.

Every firmware release publishes two byte-identical OTA assets: a numbered
`Panda-Control-Vent-<version>-OTA.bin` for archival use and the persistent
`Panda-Control-Vent-OTA.bin` filename used by the website and README.

The same application image is used when installing over compatible stock
firmware, upgrading DragonVent 0.5.9, or updating Panda Control Vent.

You need Panda Vent hardware, a browser, and a local network shared by the
controller, printer, and phone or computer. For a new Bambu connection, have
the printer's IP address, serial number, and LAN access code available. Already
configured compatible installations should not need to bind the printer again.

### Verify the download before uploading

Download `SHA256SUMS` and your chosen BIN from the **same
[release](https://github.com/rdcstout/panda-control-vent/releases/latest)**.
For the stable filename, run this command in the download folder on macOS or Linux:

```sh
shasum -a 256 Panda-Control-Vent-OTA.bin
```

On Windows PowerShell:

```powershell
Get-FileHash .\Panda-Control-Vent-OTA.bin -Algorithm SHA256
```

Compare the resulting hash with the `Panda-Control-Vent-OTA.bin` entry in
`SHA256SUMS` (letter case does not matter). They must match before uploading.
If you chose the numbered BIN, use that filename and its matching entry instead.
You do not need to download both BINs.

### From compatible stock firmware

1. Download and retain the [official Panda Vent stock firmware](https://github.com/bigtreetech/Panda-Vent/tree/master/Firmware) for recovery.
2. Open the Panda Vent's existing web interface using its IP address.
3. Open the stock firmware-update page and upload the Panda Control Vent OTA
   image.
4. Wait for the controller to restart. Do not disconnect power during the
   update.
5. If the controller was already configured, its network and printer settings
   should remain available. Otherwise, join the generated
   `Panda_Control_Vent_XXXX` setup network and complete setup.

### From DragonVent or Panda Control Vent

Open **Setup → Maintenance → Firmware update**, select the OTA image, and wait
for the interface to confirm that the controller has returned.

### Recovery and compatibility

Keep the stock image before making changes. BIGTREETECH's
[stock firmware update instructions](https://github.com/bigtreetech/Panda-Vent/blob/master/Documentation/Panda_Vent_User_Manual.md#8-ota-firmware-upgrade)
describe its factory updater. If an update has finished but the interface is
unreachable, check the controller's current IP in your router and look for its
setup network before resetting anything.

An OTA BIN is not a full-flash USB backup; do not write it to flash address zero.
Older DragonVent 0.4.x installations use a different partition layout and cannot
use this in-place upgrade path. If the device no longer boots or the layout is
uncertain, [request recovery help](https://github.com/rdcstout/panda-control-vent/issues)
before erasing flash. See the [upgrade compatibility notes](docs/UPGRADE_COMPATIBILITY.md)
for settings preservation details.

## Everyday use

- **Overview:** check printer connection and vent position, manually open or
  close the vents, or select **Auto** to follow the automation policy.
- **Automation:** choose the commanded bed-temperature threshold. The simple
  policy uses a nonzero bed target to open below that threshold or keep the
  vents closed at or above it;
  **Advanced** exposes the inherited material rules. Saving settings does not
  switch the operating mode; select Auto in Overview.
- **Lighting:** configure Vent Lights and Chamber Lights separately, including
  colors, brightness, idle dimming, and optional factory chamber-light following.

### Why are the vents still open after the print finishes?

This is intentional. In Auto, once the job has ended and the bed target is off,
Panda Control Vent uses the **actual bed temperature** to manage cooldown. Open
vents close when that temperature falls below the configured closing threshold
(35 °C by default). Cooling can take a long time after a print; there is no fixed
post-print closing timer. Overview shows the cooldown threshold so you can see
what the controller is waiting for.

The lighting sequence is separate: the Bambu completion indication lasts 7.5
seconds, then returns to the selected idle color. With **Dim while idle** enabled
in Printer status mode, the chamber lights wait another three seconds before
dimming to 20% of their selected brightness. Returning to idle does not mean the
bed is cool enough to close the vents. Manual mode does not follow this automatic
cooldown policy.

## Chamber-light modification

Download the **[Panda Control Vent printable model files on MakerWorld](https://makerworld.com/en/models/3250883-panda-control-vent#profileId-3684253)**.
The model includes a redesigned Panda Vent riser front and relocation brackets
for moving the original front LED boards into the printer as chamber lighting.
The relocation brackets attach with double-sided tape.

Each vent output contains 16 addressable pixels. Hardware probing established
the same zero-based map on both sides:

| Zone | Pixels |
| --- | ---: |
| Main vent lighting | 0–10 |
| Factory front board / relocated chamber lighting | 11–15 |

The front boards already terminate on their respective vent assemblies. No
GPIO or electrical modification is required; the physical modification is
moving those boards into the print chamber. See the
[LED hardware map](docs/LED_HARDWARE_MAP.md) for the tested wiring details.

## Known limitations

- Factory reset clears network and printer binding, but this release retains some
  lighting, calibration, access-point, and local-control settings.
- The embedded service is designed for a trusted local network. It is not an
  internet-facing service and should not be exposed through router port
  forwarding.
- Moonraker support is inherited and unverified by Extrusion Therapy.

## License and attribution

Panda Control Vent starts from
[DragonVent](https://github.com/justinh-rahb/DragonVent) v0.5.9 at commit
`51d1c1ea09e0f752b030de56f7b7a9b42fda6518`. The original MIT copyright and
license are preserved in [LICENSE](LICENSE), with additional attribution in
[NOTICE](NOTICE).

## Development and release checks

```sh
node --test tests/*.test.mjs
cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
  -Ifirmware/components/dc_bambu/include \
  tests/dc_bambu_light_parse_test.c -o /tmp/pcv-bambu-test
/tmp/pcv-bambu-test
python3 scripts/check_upgrade_contract.py
tools/idf-build.sh firmware esp32 build
```

The firmware targets the classic ESP32 using ESP-IDF 5.3.1. Build CI compiles
the source separately. The release workflow runs automated tests, verifies the
tag/version and checksums of the prebuilt BINs, confirms both filenames contain
identical bytes, and publishes them. It does not rebuild the firmware or perform
hardware testing. See [Contributing](CONTRIBUTING.md) for contribution guidance.

## Support

[Report a bug](https://github.com/rdcstout/panda-control-vent/issues) with the
firmware version, printer model, and steps to reproduce it. Follow the
[reporting checklist](CONTRIBUTING.md) and remove credentials and identifying
information from screenshots or logs.

For vulnerabilities, follow the [security reporting guidance](SECURITY.md).
Do not post exploit details or secrets publicly. If private reporting is not
available, open an issue requesting a private contact without disclosing the
vulnerability details.

Panda Control Vent is free and open source. If it helps in your shop, you can
optionally **[Support Future Tools](https://buy.stripe.com/fZu3cw2Mnfr0d7N3ws1kA00)**.
The firmware and source remain available without payment.

This project is provided as-is without a support contract or warranty.
