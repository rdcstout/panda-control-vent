import assert from 'node:assert/strict';
import test from 'node:test';
import {readFileSync,writeFileSync,mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url));
function block(s,needle){const a=s.indexOf(needle);assert.ok(a>=0,needle);let i=s.indexOf('{',a),depth=1;for(i++;depth&&i<s.length;i++){if(s[i]==='{')depth++;if(s[i]==='}')depth--;}assert.equal(depth,0);return s.slice(a,i);}
test('real startup loads saved motor calibration after one successful storage initialization',()=>{
 const main=readFileSync(join(root,'firmware/main/app_main.c'),'utf8');
 const wifi=readFileSync(join(root,'firmware/components/dc_wifi/dc_wifi.c'),'utf8');
 const motor=readFileSync(join(root,'firmware/components/dv_motor/dv_motor.c'),'utf8');
 const prefix=main.slice(main.indexOf('void app_main(void)'),main.indexOf('    ESP_ERROR_CHECK(configure_network_identity());'))+'}\n';
 const init=wifi.includes('esp_err_t dc_wifi_init_storage(')?block(wifi,'esp_err_t dc_wifi_init_storage('):'';
 const c=`
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
typedef int esp_err_t;
typedef int nvs_handle_t;
#define ESP_OK 0
#define ESP_ERR_NVS_NO_FREE_PAGES 1
#define ESP_ERR_NVS_NEW_VERSION_FOUND 2
#define NVS_READONLY 0
#define DV_MOTOR_GROUP_COUNT 4
#define CAL_NVS_NS "app_nvs"
#define CAL_NVS_KEY "hall_cal"
#define ESP_ERROR_CHECK(e) assert((e)==ESP_OK)
#define ESP_LOGI(...) ((void)0)
static bool initialized;
static int init_calls,erase_calls,init_error;
static bool s_storage_ready;
static int nvs_flash_init(void){init_calls++;if(init_error)return init_error;initialized=true;return ESP_OK;}
static int nvs_flash_erase(void){erase_calls++;return ESP_OK;}
static int nvs_open(const char *ns,int mode,nvs_handle_t *h){assert(!strcmp(ns,CAL_NVS_NS));(void)mode;*h=1;return initialized?ESP_OK:3;}
static void nvs_close(nvs_handle_t h){(void)h;}
typedef struct {int16_t open_mv,closed_mv;} hall_cal_t;
static hall_cal_t s_cal[DV_MOTOR_GROUP_COUNT];
static hall_cal_t saved[DV_MOTOR_GROUP_COUNT]={{751,1491},{755,1489},{-1,-1},{-1,-1}};
static int nvs_get_blob(nvs_handle_t h,const char *key,void *out,size_t *size){(void)h;assert(!strcmp(key,CAL_NVS_KEY));assert(*size==sizeof(saved));memcpy(out,saved,sizeof(saved));return ESP_OK;}
${init}
${block(motor,'static void cal_load(')}
static int dv_motor_init(void){cal_load();assert(!memcmp(s_cal,saved,sizeof(saved)));return ESP_OK;}
static void dc_evlog_console_init(void){}
static void dc_evlog_init(void){}
#define dc_evlog_add(...) ((void)0)
${prefix}
int main(void){
 // A failed initialization must remain retryable without erasing storage.
 ${init? 'init_error=3;assert(dc_wifi_init_storage()==3);assert(!s_storage_ready);assert(erase_calls==0);init_error=0;init_calls=0;':''}
 app_main();
 ${init? 'assert(dc_wifi_init_storage()==ESP_OK);assert(init_calls==1);assert(erase_calls==0);':'(void)init_calls;(void)erase_calls;(void)init_error;(void)s_storage_ready;(void)nvs_flash_init;(void)nvs_flash_erase;'}
 return 0;
}
`;
 const dir=mkdtempSync(join(tmpdir(),'pcv-cal-startup-'));
 try {writeFileSync(join(dir,'test.c'),c);execFileSync(process.env.CC||'cc',['-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',join(dir,'test.c'),'-o',join(dir,'test')],{stdio:'pipe'});execFileSync(join(dir,'test'),{stdio:'pipe'});}finally{rmSync(dir,{recursive:true,force:true});}
});
