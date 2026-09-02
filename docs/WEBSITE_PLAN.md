# Panda Control Vent Website Plan

## Purpose

Create a polished public software page that explains the firmware, provides the
current download, documents the optional chamber-light modification, credits
DragonVent, and routes advanced users to GitHub without creating a support desk.

Proposed public route: `https://extrusiontherapy.com/SoftwarePandaControlVent`.
This is a planned route, not a currently published page.

## Page structure

1. **Hero**
   - Use `docs/assets/panda-control-vent-hero.png`.
   - Title: `Panda Control Vent`.
   - Supporting line: `Community firmware for smarter Panda Vent control.`
   - Primary action: download the current OTA image.
   - Secondary action: view source and release notes on GitHub.

2. **What it does**
   - Bambu printer-state integration tested on an X1C.
   - Automatic temperature-based vent control.
   - Redesigned embedded control and setup interface.
   - Independent chamber lighting from the relocated front LED boards.

3. **Chamber-light modification**
   - Explain that pixels 0–10 remain vent lighting and 11–15 become chamber
     lighting on both sides.
   - Show the physical relocation with real installation photographs or a short
     video still; do not use generated wiring imagery.
   - State clearly that no GPIO reassignment or electrical rewiring is required.

4. **Download and installation**
   - Prominent 0.1.0 OTA download.
   - SHA-256 checksum link.
   - Short stock/DragonVent/Panda Control Vent upload paths.
   - Reminder to retain official stock firmware before installing.

5. **Tested and inherited support**
   - `Bambu Lab X1C — hardware tested`.
   - `Klipper/Moonraker — inherited from DragonVent, not tested by Extrusion
     Therapy`.
   - Link to the known limitations.

6. **Open-source lineage**
   - Credit Justin Hayes and DragonVent prominently.
   - Explain that generic tested Bambu corrections will be offered upstream.
   - Explain that Panda Control Vent's UI and chamber-light system define this
     fork's separate product direction.

7. **Optional support**
   - Use the established Extrusion Therapy Stripe link:
     `https://buy.stripe.com/fZu3cw2Mnfr0d7N3ws1kA00`.
   - Make clear that the download and source remain free without payment.

## Required launch assets

- [x] Wide hero image.
- [x] GitHub README copy and installation structure.
- [x] Tested firmware behavior and final 0.1.0 OTA image checksum.
- [ ] Real photographs of the installed chamber-light relocation.
- [ ] One clean screenshot each of Overview, Automation, Lighting, and Setup.
- [ ] Final public GitHub repository and release URLs.
- [ ] Optional short installation/demo video link.

## Launch verification

- Download button retrieves the exact release asset.
- Checksum matches the downloaded BIN.
- Mobile hero and install steps remain readable.
- GitHub, release, donation, DragonVent, and Extrusion Therapy links resolve.
- The page never claims independent Moonraker testing.
- No private LAN address, printer serial, access code, or Wi-Fi name appears in
  public screenshots.
