# Troubleshooting

[Back to Panda Control Vent](../README.md)

## Why are the vents still open after the print finishes?

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

## Recovery and compatibility

If the Bambu network connection is lost while the vent remains powered, the
interface reports disconnected/unknown and printer-status lighting uses its
idle appearance. The vent holds its current position until reliable printer
data returns. This does not indicate that the printer itself is idle.

Keep the stock image before making changes. BIGTREETECH's
[stock firmware update instructions](https://github.com/bigtreetech/Panda-Vent/blob/master/Documentation/Panda_Vent_User_Manual.md#8-ota-firmware-upgrade)
describe its factory updater. If an update has finished but the interface is
unreachable, check the controller's current IP in your router and look for its
setup network before resetting anything.

An OTA BIN is not a full-flash USB backup; do not write it to flash address zero.
Older DragonVent 0.4.x installations use a different partition layout and cannot
use this in-place upgrade path. If the device no longer boots or the layout is
uncertain, [request recovery help](https://github.com/rdcstout/panda-control-vent/issues)
before erasing flash. See the [upgrade compatibility notes](UPGRADE_COMPATIBILITY.md)
for settings preservation details.

## Known limitations

- Factory reset clears network and printer binding, but this release retains some
  lighting, calibration, access-point, and local-control settings.
- The embedded service is designed for a trusted local network. It is not an
  internet-facing service and should not be exposed through router port
  forwarding.
- Moonraker support is inherited and unverified by Extrusion Therapy.

## Reporting a problem

[Open an issue](https://github.com/rdcstout/panda-control-vent/issues) with your
firmware version, printer model, and steps to reproduce the problem. Follow the
[reporting checklist](../CONTRIBUTING.md); do not include credentials or private
network information. For vulnerabilities, use the [security guidance](../SECURITY.md).
