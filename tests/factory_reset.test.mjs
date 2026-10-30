import assert from 'node:assert/strict';
import test from 'node:test';
import {readFileSync, writeFileSync, mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {execFileSync} from 'node:child_process';

const root = new URL('../', import.meta.url);
function block(source, signature) {
  const start = source.indexOf(signature);
  assert.ok(start >= 0, signature);
  let end = source.indexOf('{', start), depth = 1;
  for (end++; depth && end < source.length; end++) {
    if (source[end] === '{') depth++;
    if (source[end] === '}') depth--;
  }
  assert.equal(depth, 0);
  return source.slice(start, end);
}

test('factory reset removes retained credentials and preserves lighting/calibration', () => {
  const portal = readFileSync(new URL('firmware/components/dv_portal/dv_portal.c', root), 'utf8');
  const wifi = readFileSync(new URL('firmware/components/dc_wifi/dc_wifi.c', root), 'utf8');
  const main = readFileSync(new URL('firmware/main/app_main.c', root), 'utf8');
  assert.match(block(portal, 'static esp_err_t factory_reset('), /return dv_portal_reset_settings\(\);/);
  assert.match(block(main, 'static void on_button('), /dv_portal_reset_settings\(\)/);
  const code = `
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
typedef int esp_err_t;
typedef int nvs_handle_t;
#define ESP_OK 0
#define ESP_ERR_NVS_NOT_FOUND 1
#define NVS_READWRITE 1
#define DV_NVS_NS "app_nvs"
#define NVS_NS "app_nvs"
#define KEY_SSID "ssid"
#define KEY_PASS "password"
static const char *names[] = {
 "wifi_info", "bambu_mqtt_info", "moonraker_info", "ha_mqtt_info",
 "ha_host", "ha_user", "ha_pass", "ha_port", "ap_ssid", "ap_pass",
 "ap_ip", "ap_enabled", "ap_mode", "ctl_token", "ssid", "password",
 "lighting", "hall_cal"
};
static bool present[18];
static int open_error, commit_error, closes, commits;
static const char *erase_failure;
static esp_err_t nvs_open(const char *ns, int mode, nvs_handle_t *h) {
 assert(strcmp(ns, "app_nvs") == 0); (void)mode; *h=1; return open_error;
}
static esp_err_t nvs_erase_key(nvs_handle_t h, const char *key) {
 (void)h;
 if (erase_failure && strcmp(erase_failure,key)==0) return 99;
 for (size_t i=0;i<18;i++) if(strcmp(names[i],key)==0) {
  if (!present[i]) return ESP_ERR_NVS_NOT_FOUND;
  present[i]=false; return ESP_OK;
 }
 return ESP_ERR_NVS_NOT_FOUND;
}
static esp_err_t nvs_commit(nvs_handle_t h) {(void)h; commits++;return commit_error;}
static void nvs_close(nvs_handle_t h) {(void)h;closes++;}
${block(portal, 'static esp_err_t clear_retained_setup(')}
${block(wifi, 'esp_err_t dc_wifi_clear_creds(')}
int main(void) {
 for(size_t i=0;i<18;i++) present[i]=true;
 assert(clear_retained_setup()==ESP_OK);
 for(size_t i=0;i<14;i++) assert(!present[i]);
 assert(present[14] && present[15] && present[16] && present[17]);
 assert(dc_wifi_clear_creds()==ESP_OK);
 assert(!present[14] && !present[15]);
 assert(present[16] && present[17]);
 assert(clear_retained_setup()==ESP_OK); // repeated/partially configured reset
 assert(dc_wifi_clear_creds()==ESP_OK);
 int previous_closes=closes, previous_commits=commits;
 open_error=98;
 assert(clear_retained_setup()==98);
 assert(dc_wifi_clear_creds()==98);
 assert(closes==previous_closes && commits==previous_commits);
 open_error=0; erase_failure="bambu_mqtt_info";
 assert(clear_retained_setup()==99);
 assert(commits==previous_commits);
 erase_failure="password";
 assert(dc_wifi_clear_creds()==99);
 assert(commits==previous_commits);
 erase_failure=NULL; commit_error=97;
 assert(clear_retained_setup()==97);
 assert(dc_wifi_clear_creds()==97);
 return 0;
}`;
  const dir = mkdtempSync(join(tmpdir(), 'pcv-reset-test-'));
  try {
    writeFileSync(join(dir, 'reset.c'), code);
    execFileSync('cc', ['-std=c11', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined', join(dir, 'reset.c'), '-o', join(dir, 'reset')]);
    execFileSync(join(dir, 'reset'));
  } finally {
    rmSync(dir, {recursive:true, force:true});
  }
});
