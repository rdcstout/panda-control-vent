# Moonraker component override

Copied from `justinh-rahb/dragon-core`, commit
`8d5a61de8544a5b82cc3c6d9c7dc593ce06bb8bd`, `components/dc_moonraker`.
Covered by the original MIT license and attribution in the repository LICENSE
and NOTICE. Keep protocol/client behavior aligned with that pinned source.

Only runtime change: `dc_moonraker_get_status` safely returns its initial disabled
snapshot and `ESP_ERR_INVALID_STATE` before startup. Selecting Klipper after a Bambu boot can call this getter
from policy, API or lighting before a Moonraker mutex exists; the original getter
asserted on the null semaphore and rebooted the Vent. No new client is started.
The local component takes precedence over the pinned managed component.
