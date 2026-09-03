# Installation and updates

[Back to Panda Control Vent](../README.md)

## Before you install

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

### Updating Panda Control Vent

In Panda Control Vent, open **Setup → Maintenance → Firmware update**, select the OTA image, and wait
for the interface to confirm that the controller has returned.

For connection problems or a device that does not return after updating, see
[Troubleshooting](TROUBLESHOOTING.md#recovery-and-compatibility).

## After installation

- **Overview:** check printer connection and vent position, manually open or
  close the vents, or select **Auto** to follow the automation policy.
- **Automation:** choose the commanded bed-temperature threshold. The simple
  policy uses a nonzero bed target to open below that threshold or keep the
  vents closed at or above it. **Advanced** exposes the inherited material rules.
  Saving settings does not switch the operating mode; select Auto in Overview.
- **Lighting:** configure Vent Lights and Chamber Lights separately, including
  colors, brightness, idle dimming, and optional factory chamber-light following.
