import assert from 'node:assert/strict';
import test from 'node:test';
import {readFileSync,writeFileSync,mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url));
function block(s,needle){const a=s.indexOf(needle);assert.ok(a>=0,needle);let i=s.indexOf('{',a),depth=1;for(i++;depth&&i<s.length;i++){if(s[i]==='{')depth++;if(s[i]==='}')depth--;}assert.equal(depth,0);return s.slice(a,i);}
test('real Moonraker getter is safe before startup and still locks live snapshots',()=>{
 const component=join(root,'firmware/components/dc_moonraker');
 const source=readFileSync(join(component,'dc_moonraker.c'),'utf8');
 const c=`
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "dc_moonraker.h"
#define portMAX_DELAY 0
static void *s_lock;
static dc_moonraker_status_t s_status={.state=DC_MK_DISABLED,.chamber_temp=NAN};
static int takes,gives;
static void xSemaphoreTake(void *lock,int delay){assert(lock!=NULL);(void)delay;takes++;}
static void xSemaphoreGive(void *lock){assert(lock!=NULL);gives++;}
${block(source,'esp_err_t dc_moonraker_get_status(')}
int main(void){
 dc_moonraker_status_t out;
 assert(dc_moonraker_get_status(NULL)==ESP_ERR_INVALID_ARG);
 memset(&out,0xff,sizeof(out));
 assert(dc_moonraker_get_status(&out)==ESP_ERR_INVALID_STATE);
 assert(out.state==DC_MK_DISABLED && out.printer==DC_PRINTER_UNKNOWN && !out.printing && isnan(out.chamber_temp));
 assert(takes==0 && gives==0);
 s_lock=(void *)1;s_status.state=DC_MK_SUBSCRIBED;s_status.printer=DC_PRINTER_PRINTING;s_status.bed_temp=70;s_status.bed_target=70;strcpy(s_status.material,"PETG");
 assert(dc_moonraker_get_status(&out)==ESP_OK);assert(out.state==DC_MK_SUBSCRIBED && out.printer==DC_PRINTER_PRINTING && out.bed_target==70 && !strcmp(out.material,"PETG"));assert(takes==1 && gives==1);
 return 0;
}`;
 const dir=mkdtempSync(join(tmpdir(),'pcv-moonraker-unstarted-'));
 try {writeFileSync(join(dir,'esp_err.h'),'#pragma once\ntypedef int esp_err_t;\n#define ESP_OK 0\n#define ESP_ERR_INVALID_ARG 1\n#define ESP_ERR_INVALID_STATE 2\n');writeFileSync(join(dir,'test.c'),c);execFileSync(process.env.CC||'cc',['-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+dir,'-I'+join(component,'include'),join(dir,'test.c'),'-o',join(dir,'test')],{stdio:'pipe'});execFileSync(join(dir,'test'),{stdio:'pipe'});}finally{rmSync(dir,{recursive:true,force:true});}
});
