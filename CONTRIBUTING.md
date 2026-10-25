# Contributing

Panda Control Vent is a small, workshop-developed community project. Focused
bug reports and narrowly scoped pull requests are welcome, but the project does
not provide a support contract or guaranteed response time.

Before opening an issue:

- confirm the controller is running a published Panda Control Vent release;
- remove Wi-Fi passwords, Bambu access codes, control tokens, serial numbers,
  and other private network information from screenshots and logs;
- include the firmware version, printer model, control source, and exact steps
  that reproduce the behavior; and
- state whether the problem also occurs after a controller reboot.

Pull requests should preserve the stock-compatible partition layout and NVS
upgrade contract. Run the checks below before submitting.

Changes proposed for DragonVent follow the separate
[upstream contribution plan](docs/UPSTREAM_CONTRIBUTION_PLAN.md). Chamber-light
features and the Panda Control Vent interface are not part of upstream patches.

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
hardware testing.

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

Error parsing and mapping are covered by host-side tests; a genuine printer
fault was not induced during hardware testing.

Release `0.1.1` adds Bambu state-handling and reconnect corrections, verified by
automated tests and a successful firmware build. That verification is separate
from the baseline's extended hardware testing. See the
[release notes](https://github.com/rdcstout/panda-control-vent/releases/latest)
for changes in the current download.

Klipper/Moonraker support is inherited from DragonVent. It remains available,
but Extrusion Therapy has not independently hardware-tested that path.

## Review and updates

Coordinate substantial features, new dependencies, architectural changes, or changes to defaults with the maintainer before implementation. Keep each PR focused, explain compatibility and user-data effects, and report exactly which checks passed, failed, or remain unavailable. A build alone does not prove UI or hardware behavior.

Do not push merely to keep pace with the default branch. Update for conflicts, actionable review feedback, or a substantive improvement within the agreed scope. Preserve other contributors' work; do not force-push the default branch or rewrite shared history without coordination. Avoid duplicate CI runs and retry only bounded, verified infrastructure failures.

Passing checks does not guarantee acceptance or a release. Merging and publication remain maintainer decisions; contributors must not enable auto-merge, merge, or publish on the maintainer's behalf without explicit authorization. Review and support are best effort, with no guaranteed response schedule.

Use source and assets you have the right to contribute, preserve existing license and third-party terms, and discuss unclear contribution rights before submitting. Never commit secrets, personal settings, generated installers, build caches, or unrelated work.

## Development and CI references

Use [the existing development checks](CONTRIBUTING.md#development-and-release-checks) and include results appropriate to the change. The current workflow definitions are [firmware-build.yml](.github/workflows/firmware-build.yml), [firmware-release.yml](.github/workflows/firmware-release.yml). Consult their actual triggers, path filters, jobs, and approval behavior before requesting CI; this policy does not enable or change workflows. CI build/test results do not replace packaged-app or hardware acceptance.

Report suspected vulnerabilities through [SECURITY.md](SECURITY.md), rather than public issues or PRs.

Firmware and release checks retain the existing partition/NVS and hardware-verification requirements above. Push a release tag or invoke the publishing workflow only with explicit maintainer authorization.
