# scripts

Small compatibility helpers for backing up, recovering, and diagnosing Panda
Control Vent hardware.

The old `openvent` and `openvent-diag` names remain as compatibility wrappers
during the repository rebrand.

## `dragonvent`

Flashing and backup wrapper retained under its inherited command name. See
`dragonvent help`.

```sh
scripts/dragonvent backup                    # dump the whole flash to a .bin
scripts/dragonvent restore my-backup.bin     # flash a backup back
scripts/dragonvent install -t v0.1.0-rc.10   # download + upload over stock
```

## `dragonvent-diag` — capture motor diagnostic logs

Use this if the vent motors are misbehaving (running too long, not stopping at
an endpoint, moving the wrong direction, etc.). It streams the ESP32's serial
log, saves everything to a timestamped file, and prints the motor lines in
real time so you can watch what's happening.

Requires `pyserial`:

```sh
python3 -m pip install --user pyserial
```

Then, with the device on USB:

```sh
scripts/dragonvent-diag           # auto-detects the port
scripts/dragonvent-diag -p /dev/tty.usbserial-0001   # or specify one
```

**What to do while it's running:**

1. Load the Panda Control Vent portal using its current IP address or AP IP.
2. Go to the Home tab and click **Open vent**. Wait at least 5 seconds even
   if the motor doesn't stop — the retry loop is what we want to see.
3. Click **Close vent** and wait for it to close.
4. Repeat a few times if you like.
5. `Ctrl-C` to finish.

The script prints a summary of the raw ADC values seen at each endpoint, and
writes the full serial log to `dragonvent-log-YYYYMMDD-HHMMSS.txt` in the
current directory. Share that file back to the maintainer (attach to an issue
or Discord message) so we can pick hall-sensor thresholds that actually match
your board.
