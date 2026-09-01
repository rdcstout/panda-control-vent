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
upgrade contract. Run the checks listed in the root README before submitting.

Changes proposed for DragonVent follow the separate
[upstream contribution plan](docs/UPSTREAM_CONTRIBUTION_PLAN.md). Chamber-light
features and the Panda Control Vent interface are not part of upstream patches.
