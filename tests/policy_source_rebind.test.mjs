import assert from 'node:assert/strict';
import test from 'node:test';
import {readFileSync, writeFileSync, mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';
const root = fileURLToPath(new URL('../', import.meta.url));
function block(source, needle) {
  const start = source.indexOf(needle);
  assert.ok(start >= 0, needle);
  let i = source.indexOf('{', start), depth = 1;
  for (i++; depth && i < source.length; i++) {
    if (source[i] === '{') depth++;
    if (source[i] === '}') depth--;
  }
  assert.equal(depth, 0);
  return source.slice(start, i);
}
test('real policy follows source rebinding without reboot and preserves safety/manual/cooldown rules', () => {
  const source = readFileSync(join(root, 'firmware/components/dv_policy/dv_policy.c'), 'utf8');
  let c = readFileSync(new URL('fixtures/policy_source_rebind.c', import.meta.url), 'utf8');
  // Include the real globals too: the old boot-time source cache must reproduce.
  c = c.replace('/* INJECT:globals */', source.slice(source.indexOf('typedef enum {'), source.indexOf('static void read_auto_input(')));
  for (const [name, prefix] of Object.entries({read_auto_input:'static void ', apply_target:'static void ', decide_auto_target:'static dv_motor_target_t ', policy_task:'static void ', dv_policy_start:'esp_err_t '})) {
    c = c.replace('/* INJECT:' + name + ' */', block(source, prefix + name + '('));
  }
  assert.ok(!c.includes('/* INJECT:'));
  const dir = mkdtempSync(join(tmpdir(), 'pcv-policy-rebind-'));
  try {
    writeFileSync(join(dir, 'esp_err.h'), '#pragma once\ntypedef int esp_err_t;\n#define ESP_OK 0\n#define ESP_ERR_INVALID_STATE 1\n#define ESP_ERR_NO_MEM 2\n');
    writeFileSync(join(dir, 'test.c'), c);
    execFileSync(process.env.CC || 'cc', ['-std=c11', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined', '-I'+dir, '-I'+join(root, 'firmware/components/dc_bambu/include'), '-I'+join(root, 'firmware/components/dv_policy/include'), '-I'+join(root, 'firmware/components/dv_motor/include'), join(dir, 'test.c'), '-o', join(dir, 'test')], {stdio:'pipe'});
    console.log(execFileSync(join(dir, 'test'), {encoding:'utf8'}));
  } finally { rmSync(dir, {recursive:true, force:true}); }
});
