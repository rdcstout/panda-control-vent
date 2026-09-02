import assert from 'node:assert/strict';
import test from 'node:test';
import { readFile } from 'node:fs/promises';

const main = await readFile(new URL('../firmware/main/app_main.c', import.meta.url), 'utf8');
const portal = await readFile(new URL('../firmware/components/dv_portal/dv_portal.c', import.meta.url), 'utf8');
const bambu = await readFile(new URL('../firmware/components/dc_bambu/dc_bambu.c', import.meta.url), 'utf8');
const rgb = await readFile(new URL('../firmware/components/dv_rgb/dv_rgb.c', import.meta.url), 'utf8');

test('Bambu detailed phases reach the RGB status mapper', () => {
  assert.match(main, /DC_BAMBU_PRINT_PREPARING:\s+status = DV_PS_PREPARING/);
  assert.match(main, /DC_BAMBU_PRINT_PRINTING:\s+status = DV_PS_PRINTING/);
  assert.match(main, /DC_BAMBU_PRINT_PAUSED:\s+status = DV_PS_PAUSED/);
  assert.match(main, /DC_BAMBU_PRINT_COMPLETE:\s+status = DV_PS_COMPLETE/);
  assert.match(main, /DC_BAMBU_PRINT_ERROR:\s+status = DV_PS_ERROR/);
});

test('Bambu detailed phases remain visible in the API', () => {
  assert.match(portal, /DC_BAMBU_PRINT_PAUSED:\s+return "paused"/);
  assert.match(portal, /DC_BAMBU_PRINT_COMPLETE:\s+return "complete"/);
  assert.match(portal, /printer_state = bambu_print_wire\(status\.print_state\)/);
});

test('Bambu completion is a one-shot state that expires to idle', () => {
  assert.match(bambu, /COMPLETION_HOLD_US\s+7500000LL/);
  assert.match(bambu, /s_has_observed_active_job\s*=\s*true/);
  assert.match(bambu, /s_completion_until_us\s*=\s*now_us\s*\+\s*COMPLETION_HOLD_US/);
  assert.match(bambu, /static void expire_completion_locked/);
  assert.match(bambu, /s_status\.print_state\s*=\s*DC_BAMBU_PRINT_IDLE/);
  assert.equal((bambu.match(/now_us\s*<\s*s_completion_until_us/g) ?? []).length, 1);
});

test('idle dimming waits after every real job-ending transition', () => {
  assert.match(rgb, /IDLE_DIM_DELAY_US\s+3000000LL/);
  assert.match(rgb, /status == DV_PS_IDLE && s_pstatus != DV_PS_NONE && s_pstatus != DV_PS_IDLE/);
  assert.match(rgb, /s_idle_dim_after_us\s*=\s*esp_timer_get_time\(\)\s*\+\s*IDLE_DIM_DELAY_US/);
  assert.match(rgb, /idle && delay_elapsed/);
  assert.match(rgb, /\(cfg->brightness \+ 2u\) \/ 5u/);
});
