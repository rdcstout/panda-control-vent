import assert from 'node:assert/strict';
import test from 'node:test';
import { readFileSync, writeFileSync, mkdtempSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { execFileSync } from 'node:child_process';

function functionBlock(source, name) {
  const start = source.indexOf(name);
  assert.ok(start >= 0, name);
  let end = source.indexOf('{', start), depth = 1;
  for (end++; depth && end < source.length; end++) {
    if (source[end] === '{') depth++;
    if (source[end] === '}') depth--;
  }
  assert.equal(depth, 0);
  return source.slice(start, end);
}

test('production Wi-Fi handlers keep recovering after a runtime outage and rearm DHCP watchdog', () => {
  const source = readFileSync(new URL('../firmware/components/dc_wifi/dc_wifi.c', import.meta.url), 'utf8');
  const dir = mkdtempSync(join(tmpdir(), 'pcv-wifi-recovery-'));
  const code = `
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#define WIFI_EVENT 1
#define IP_EVENT 2
#define WIFI_EVENT_STA_START 10
#define WIFI_EVENT_STA_CONNECTED 11
#define WIFI_EVENT_STA_DISCONNECTED 12
#define WIFI_EVENT_SCAN_DONE 13
#define IP_EVENT_STA_GOT_IP 20
#define DC_WIFI_STATE_STA_CONNECTING 1
#define DC_WIFI_STATE_STA_CONNECTED 2
#define DC_WIFI_STATE_AP_PORTAL 3
#define STA_MAX_RETRIES 20
#define BIT_CONNECTED 1
#define BIT_FAILED 2
#define NO_IP_TIMEOUT_US 5000000ULL
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
typedef int esp_event_base_t;
typedef struct { unsigned char reason; } wifi_event_sta_disconnected_t;
typedef struct { int ip_info; } ip_event_got_ip_t;
static int s_state=DC_WIFI_STATE_STA_CONNECTING, s_retry, s_events;
static bool s_sta_failed=true;
${source.includes('static bool s_sta_ever_connected') ? 'static bool s_sta_ever_connected;' : ''}
static unsigned char s_last_disc_reason;
static int connect_calls, disconnect_calls, watch_starts, watch_stops, bits;
static int esp_wifi_connect(void) { connect_calls++; return 0; }
static int esp_wifi_disconnect(void) { disconnect_calls++; return 0; }
static void no_ip_watch_start(void) { watch_starts++; }
static void no_ip_watch_stop(void) { watch_stops++; }
static void handle_scan_done(void) {}
static void xEventGroupSetBits(int events,int value) { (void)events; bits|=value; }
static void xEventGroupClearBits(int events,int value) { (void)events; bits&=~value; }
${functionBlock(source,'static void no_ip_timeout_cb(')}
${functionBlock(source,'static void on_wifi_event(')}
${functionBlock(source,'static void on_ip_event(')}
int main(void) {
  wifi_event_sta_disconnected_t reason={200};
  // Existing first-boot retry limit and setup fallback signal remain unchanged.
  for(int i=0;i<STA_MAX_RETRIES+1;i++) on_wifi_event(0,WIFI_EVENT,WIFI_EVENT_STA_DISCONNECTED,&reason);
  assert(connect_calls==STA_MAX_RETRIES); assert(bits & BIT_FAILED);
  // Successfully joining changes subsequent disconnects into runtime recovery.
  ip_event_got_ip_t ip={0}; on_ip_event(0,IP_EVENT,IP_EVENT_STA_GOT_IP,&ip);
  assert(s_state==DC_WIFI_STATE_STA_CONNECTED); assert(s_retry==0); assert(!s_sta_failed);
  connect_calls=0; bits=BIT_CONNECTED;
  on_wifi_event(0,WIFI_EVENT,WIFI_EVENT_STA_DISCONNECTED,&reason);
  assert(s_state==DC_WIFI_STATE_STA_CONNECTING); assert(!(bits & BIT_CONNECTED));
  on_wifi_event(0,WIFI_EVENT,WIFI_EVENT_STA_CONNECTED,0);
  assert(watch_starts>0); no_ip_timeout_cb(0); assert(disconnect_calls==1);
  // An outage lasting beyond the initial provisioning limit must remain retryable.
  for(int i=0;i<STA_MAX_RETRIES+5;i++) on_wifi_event(0,WIFI_EVENT,WIFI_EVENT_STA_DISCONNECTED,&reason);
  assert(connect_calls==STA_MAX_RETRIES+6); assert(!(bits & BIT_FAILED));
  on_ip_event(0,IP_EVENT,IP_EVENT_STA_GOT_IP,&ip);
  assert(s_state==DC_WIFI_STATE_STA_CONNECTED); assert(s_retry==0); assert(bits & BIT_CONNECTED);
  no_ip_timeout_cb(0); assert(disconnect_calls==1); // Ignore stale watchdog after IP arrives.
  s_state=DC_WIFI_STATE_AP_PORTAL; connect_calls=0;
  on_wifi_event(0,WIFI_EVENT,WIFI_EVENT_STA_DISCONNECTED,&reason);
  assert(connect_calls==0); // Setup AP does not start a competing reconnect loop.
  (void)watch_stops; (void)xEventGroupClearBits;
  return 0;
}
`;
  try {
    writeFileSync(join(dir, 'test.c'), code);
    execFileSync(process.env.CC || 'cc', ['-std=c11', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter', '-Wno-unused-variable', '-fsanitize=address,undefined', join(dir, 'test.c'), '-o', join(dir, 'test')], { stdio: 'pipe' });
    execFileSync(join(dir, 'test'), { stdio: 'pipe' });
  } finally { rmSync(dir, { recursive: true, force: true }); }
});
