# LED Hardware Map

This map was established with a temporary raw-address probe running on the
actual Panda Vent hardware on 2026-08-27.

## Confirmed physical outputs

| Firmware output | GPIO | Physical side |
| --- | ---: | --- |
| Output 0 | 14 | Right |
| Output 1 | 4 | Left |

## Confirmed chamber-light addresses

The small factory front LED boards terminate on their respective vent assembly.
After relocating those boards inside the printer, they become chamber lights.

| Logical zone | Output 0 / right | Output 1 / left |
| --- | --- | --- |
| Chamber lights | pixels 11-15 | pixels 11-15 |

Pixels 11 through 15 illuminated only the front board on each tested output.
This proves that the boards are independently addressable in software; no GPIO
or wiring modification is required.

## Zone behavior

- Both physical chamber boards normally act as one logical `chamber` zone.
- The chamber zone supports the same enable, brightness, color, effect, speed,
  printer-state, vent-state, temperature, and error behavior as the vent zone.
- `linked` mode preserves the original whole-string behavior.
- `independent` mode renders a separate chamber configuration.
- Pixel classification must occur before effect-direction reversal so reversing
  an animation cannot move chamber behavior onto vent addresses.

## Validation status

The hardware walk confirmed pixels 0 through 10 as the main vent lighting and
pixels 11 through 15 as the relocated front/chamber-light board on both outputs.
The release mapping is complete for the tested retail hardware.
