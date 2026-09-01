# Upstream Contribution Plan

Panda Control Vent is a public MIT-licensed fork of DragonVent. We will return
general Bambu compatibility corrections to DragonVent while keeping the fork's
separate product direction intact.

## Contribute to DragonVent

Pull requests to DragonVent may include only changes needed to make its existing
firmware and interface work correctly with Bambu printers:

- parsing Bambu preparing, printing, paused, completed, failed, stopped, error,
  and idle states;
- parsing numeric and quoted decimal/hex `print_error` values;
- distinguishing a deliberate user stop from a genuine printer error;
- retaining pause until the printer resumes or the job ends;
- displaying completion briefly and then returning to idle instead of holding
  stale `FINISH` forever;
- preventing a stale completion reported at startup from creating a false
  completed event;
- tests covering those state transitions; and
- factual notes that the changes were exercised against a Bambu Lab X1C.

Each behavior should be submitted as a small, reviewable pull request against
the current DragonVent baseline. The upstream patch must be recreated from the
smallest necessary diff; do not submit the Panda Control Vent branch wholesale.

## Keep in Panda Control Vent

The following are intentionally excluded from every DragonVent pull request:

- all chamber-light parsing, state, configuration, persistence, APIs, UI,
  documentation, and tests;
- the independent pixel-zone implementation and the 0–10 / 11–15 mapping;
- factory chamber-light following, idle dimming, and delayed dimming;
- the replacement embedded interface and Panda Control visual system;
- Panda Control Vent naming, hostname, mDNS identity, and setup-AP branding;
- the simplified Bambu automation policy and its basic/advanced presentation;
- Panda Control desktop-app integration and fork-specific API additions;
- release presentation, website assets, and Extrusion Therapy support links.

## Review gate

Before opening an upstream pull request:

1. Diff it against pristine DragonVent, not against the complete fork.
2. Search the patch for `chamber`, `dim_idle`, `follow_printer_light`, pixel-zone
   definitions, Panda Control branding, and fork-specific API fields.
3. Run the Bambu parser and state tests.
4. Confirm that no embedded-interface replacement or chamber-light behavior is
   present.
5. Present the patch for final approval before pushing it to DragonVent.
