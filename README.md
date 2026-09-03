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

## What it does

Panda Control Vent adds automatic vent control, printer-status lighting, and
independent chamber lighting to the BIGTREETECH Panda Vent, with a browser-based
interface for setup and everyday control.

- Bambu LAN integration for printer status and temperature-based automation.
- Separate vent and chamber-light colors, brightness, effects, and idle dimming.
- Optional chamber lights that follow the printer's factory chamber light.
- In-place firmware updates that preserve compatible existing settings.
- Advanced material rules for Klipper/Moonraker users.

Developed and hardware-tested with a Bambu Lab X1C. Klipper/Moonraker support
is inherited and has not been independently hardware-tested by Extrusion Therapy.
See [testing details](CONTRIBUTING.md#tested-configuration) for coverage and limits.

## Get started

You need a BIGTREETECH Panda Vent, a browser, and a shared local network for
the controller, printer, and phone or computer.

**[Download the latest firmware](https://github.com/rdcstout/panda-control-vent/releases/latest/download/Panda-Control-Vent-OTA.bin)**,
then follow the **[installation and update guide](docs/INSTALLATION.md)**.
Keep an official stock firmware image for recovery; the guide links to it.

Use **Overview** for manual vent control or Auto, **Automation** for temperature
thresholds and advanced rules, and **Lighting** for the vent and chamber lights.

> **Vents still open after printing?** In Auto, after the job ends and the bed
> target is off, the vents can remain open until the actual bed temperature falls
> below the configured closing threshold (35 °C by default). This can take a long
> time. [Cooldown and lighting explained](docs/TROUBLESHOOTING.md#why-are-the-vents-still-open-after-the-print-finishes).

## Chamber-light modification

The [printable model files](https://makerworld.com/en/models/3250883-panda-control-vent#profileId-3684253)
include a redesigned riser front and brackets for relocating the original front
LED boards inside the chamber. The brackets attach with double-sided tape.
No GPIO or electrical modification is required. See the
[LED hardware map](docs/LED_HARDWARE_MAP.md) for the tested wiring and pixel layout.

## Help and development

- [Installation and updates](docs/INSTALLATION.md)
- [Troubleshooting and recovery](docs/TROUBLESHOOTING.md)
- [Report a bug](https://github.com/rdcstout/panda-control-vent/issues)
- [Security reporting](SECURITY.md) — never post credentials or vulnerability details publicly.
- [Contributing, build instructions, and testing](CONTRIBUTING.md)

This firmware is intended for a trusted local network; do not expose its web
interface through internet port forwarding. It is provided as-is, without a
support contract or warranty.

## License and attribution

Unofficial community firmware, not endorsed by BIGTREETECH or Bambu Lab.
Based on [DragonVent](https://github.com/justinh-rahb/DragonVent) v0.5.9.
The original MIT license and copyright are preserved in [LICENSE](LICENSE);
see [NOTICE](NOTICE) for attribution.

Panda Control Vent is free and open source.
[Support Future Tools](https://buy.stripe.com/fZu3cw2Mnfr0d7N3ws1kA00) is optional;
downloads and source require no payment.
