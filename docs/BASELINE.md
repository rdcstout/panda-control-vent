# Development Baseline

- Product: Panda Control Vent
- Current tested stable release: 0.1.0-rc.10
- Upstream: DragonVent v0.5.9
- Upstream commit: `51d1c1ea09e0f752b030de56f7b7a9b42fda6518`
- Dragon Core: v0.30.0, pinned to object
  `8d5a61de8544a5b82cc3c6d9c7dc593ce06bb8bd`
- Target: ESP32
- Partition strategy: stock-compatible OTA application slots

The fork begins from committed upstream source only. The temporary LED probe is
not part of this baseline.

Release verification requires:

1. the build reports the intended Panda Control Vent release version;
2. the output is a valid ESP32 OTA application image;
3. the image fits the stock application partition;
4. normal Wi-Fi, printer source, vent policy, portal, status LED, RGB, and button
   startup services remain present;
5. a known-good DragonVent v0.5.9 OTA recovery image is available locally.
