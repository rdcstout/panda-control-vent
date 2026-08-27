# Design QA — Panda Control Vent 0.1.0-rc.1

**Source visual truth**

- `design-qa-assets/source-panda-control.png`
- Original pixels: 1487 × 1058.

**Rendered implementation**

- Primary comparison: `design-qa-assets/implementation-overview-light.png`
- Additional states: `implementation-overview-dark.png`,
  `implementation-lighting-dark.png`, `implementation-setup-light.png`,
  `implementation-setup-dark.png`, and `implementation-setup-compact.png`.
- Combined comparison: `design-qa-assets/combined-comparison.png`.

**Viewport and normalization**

- Desktop CSS viewport: 1440 × 1000 at device scale factor 1.
- Primary implementation capture: 1440 × 1000 pixels.
- Compact CSS viewport: 800 × 900 at device scale factor 1; full-page capture
  785 × 2520 pixels after the browser scrollbar gutter.
- The source and desktop implementation were displayed side by side at equal
  column widths in the combined comparison. Browser chrome was excluded.

**State**

- Overview in light mode is the primary source comparison.
- Overview, independent Lighting, and full Setup were also checked in dark mode.
- Setup was checked in light mode at desktop and compact widths.

**Full-view comparison evidence**

- The implementation carries forward the selected source's top-tab navigation,
  restrained red accent, neutral Apple-like typography, wide content frame, and
  quiet status treatment.
- The requested additional shape is present through distinct background and
  surface tokens, card borders, modest corner radii, and restrained elevation.
- The embedded product intentionally uses cards rather than the source's long
  flat settings table because Overview, Lighting, and Setup have different
  information densities. This is a product adaptation, not unexplained drift.
- Light and dark modes retain hierarchy and contrast without gradients or gaudy
  decoration.

**Focused-region evidence**

- `implementation-lighting-dark.png` confirms that Vent Lights and Chamber
  Lights have equal control depth and that the link/unlink control is prominent.
- `implementation-setup-light.png` confirms the simplified printer connection,
  Wi-Fi, Setup Access Point, maintenance, and recovery hierarchy. The setup AP
  copy describes behavior in user terms and does not expose gateway/mode jargon.
- The source does not contain equivalent lighting or setup states, so those
  regions were evaluated against the approved visual language and task flow
  rather than falsely treated as pixel-identical source screens.

**Required fidelity surfaces**

- Fonts and typography: system UI stack, optical weight hierarchy, wrapping,
  and small-label contrast are consistent with the source direction.
- Spacing and layout rhythm: card gaps, control alignment, radii, shadows, and
  vertical rhythm are consistent at desktop and compact widths.
- Colors and tokens: neutral surfaces, restrained red actions, and semantic
  green/amber/error states remain coherent in both themes.
- Image and asset fidelity: neither the source nor implementation depends on
  product imagery or decorative illustration; no source asset was replaced with
  a code-drawn approximation.
- Copy and content: product-specific labels are plain-language and operational;
  Bambu setup promises live verification without reboot, and setup-AP behavior
  is described accurately.

**Interactions and browser checks**

- Tested Overview, Automation, Lighting, and Setup tab navigation.
- Tested theme switching and persistent light/dark state.
- Tested lighting-zone render and link-state behavior in demo mode.
- Verified desktop and compact layouts have no horizontal overflow.
- Browser console warnings/errors checked: none.

**Findings**

- No actionable P0, P1, or P2 design differences remain.
- P3 follow-up: hardware testing may reveal copy refinements for transient Bambu
  connection states, but the current labels are complete and internally consistent.

**Comparison history**

- Pass 1: no actionable P0/P1/P2 findings; no visual fix iteration was required.

**Implementation checklist**

- Hardware-test the live Bambu connection confirmation.
- Hardware-test linked and independent lighting persistence after reboot.
- Hardware-test setup-AP fallback by making the saved Wi-Fi temporarily unavailable.

final result: passed
