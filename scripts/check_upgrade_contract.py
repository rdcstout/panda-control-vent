#!/usr/bin/env python3
"""Fail the release if the DragonVent 0.5.9 in-place upgrade contract drifts."""

from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BASELINE = "51d1c1e"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"upgrade contract failed: {message}")


def read(relative: str) -> str:
    return (ROOT / relative).read_text(encoding="utf-8")


baseline_partitions = subprocess.check_output(
    ["git", "show", f"{BASELINE}:firmware/partitions.csv"],
    cwd=ROOT,
    text=True,
)
require(
    read("firmware/partitions.csv") == baseline_partitions,
    "the OTA/NVS partition layout no longer matches DragonVent 0.5.9",
)

component_manifest = read("firmware/main/idf_component.yml")
for component in ("dc_evlog", "dc_source", "dc_moonraker", "dc_portal"):
    require(f"  {component}:" in component_manifest, f"missing {component} dependency")
require(component_manifest.count("version: v0.30.0") == 4, "shared dependencies are not pinned to v0.30.0")

expected_keys = {
    "firmware/components/dc_wifi/dc_wifi.c": (
        'KEY_SSID    "ssid"', 'KEY_PASS    "password"', 'KEY_AP_MODE "ap_mode"',
    ),
    "firmware/components/dc_bambu/dc_bambu.c": (
        'KEY_HOST "bb_host"', 'KEY_SER  "bb_serial"', 'KEY_CODE "bb_code"',
    ),
    "firmware/managed_components/dc_source/dc_source.c": ('KEY_SRC "ctl_src"',),
    "firmware/components/dv_policy/dv_policy.c": (
        'KEY_MODE       "policy_mode"', 'KEY_BED_OPEN   "bed_open_c"',
        'KEY_BED_CLOSE  "bed_close_c"', 'KEY_MAN_TGT    "man_tgt"',
        'KEY_FILAMENT   "fil_rules"',
    ),
    "firmware/components/dv_rgb/dv_rgb.c": ('CFG_NVS_KEY "lighting"',),
    "firmware/components/dv_motor/dv_motor.c": ('CAL_NVS_KEY  "hall_cal"',),
}
for relative, keys in expected_keys.items():
    source = read(relative)
    for key in keys:
        require(key in source, f"persistence key changed or disappeared: {key}")

header = read("firmware/components/dv_rgb/include/dv_rgb.h")
require(
    header.index("uint8_t rev_strip[2]") < header.index("bool chamber_independent") < header.index("dv_lighting_profile_t chamber"),
    "new chamber-light fields are no longer appended after the DragonVent 0.5.9 lighting prefix",
)

ota = read("firmware/managed_components/dc_portal/dc_portal.c")
require("esp_ota_get_next_update_partition(NULL)" in ota, "OTA no longer targets only the inactive app partition")
require("nvs_flash_erase" not in ota, "OTA handler now erases NVS")

print("upgrade contract passed: DragonVent 0.5.9 settings remain in place")
