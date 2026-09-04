import assert from 'node:assert/strict';
import test from 'node:test';
import {readFileSync,writeFileSync,mkdtempSync,rmSync,existsSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url));
function block(s,needle){
 const a=s.indexOf(needle);assert.ok(a>=0,needle);
 let i=s.indexOf('{',a),depth=1;
 for(i++;depth&&i<s.length;i++){if(s[i]==='{')depth++;if(s[i]==='}')depth--;}
 assert.equal(depth,0);return s.slice(a,i);
}
test('live Bambu mapping and real lighting selection cover every phase offline and online',()=>{
 const main=readFileSync(join(root,'firmware/main/app_main.c'),'utf8');
 const portal=readFileSync(join(root,'firmware/components/dv_portal/dv_portal.c'),'utf8');
 const policy=readFileSync(join(root,'firmware/components/dv_policy/dv_policy.c'),'utf8');
 const rgb=readFileSync(join(root,'firmware/components/dv_rgb/dv_rgb.c'),'utf8');
 assert.ok(portal.includes('printer_state = dv_bambu_live_state_str(&status)'));
 assert.ok(policy.includes('out->state = dv_bambu_live_state_str(&st)'));
 const mapping=block(main,'switch (dv_bambu_live_phase(&st))');
 const renderer=block(rgb,'static const uint8_t *printer_status_color(');
 const include=[process.env.BAMBU_INCLUDE_DIR,join(root,'firmware/components/dc_bambu/include'),join(root,'firmware/managed_components/dc_bambu/include')].find(p=>p&&existsSync(join(p,'dc_bambu.h')));
 assert.ok(include,'Bambu dependency headers must be installed by the firmware build');
 const code=[
 '#include <assert.h>','#include <stdint.h>','#include <string.h>','#include "dv_bambu_state.h"',
 'typedef enum {DV_PS_NONE,DV_PS_IDLE,DV_PS_PREPARING,DV_PS_PRINTING,DV_PS_PAUSED,DV_PS_COMPLETE,DV_PS_ERROR} dv_printer_status_t;',
 'static dv_printer_status_t s_pstatus;',
 'static struct {uint8_t idle[3],prep[3],printing[3],paused[3],complete[3],error[3];} s_cfg;',
 renderer,
 'static dv_printer_status_t lighting(dc_bambu_status_t st){dv_printer_status_t status=DV_PS_NONE;',
 mapping,'return status;}',
 'int main(void){',
 'const char *names[]={"idle","idle","downloading","preparing","printing","paused","complete","error"};',
 'const dv_printer_status_t lights[]={DV_PS_IDLE,DV_PS_IDLE,DV_PS_PREPARING,DV_PS_PREPARING,DV_PS_PRINTING,DV_PS_PAUSED,DV_PS_COMPLETE,DV_PS_ERROR};',
 'for(int p=0;p<=DC_BAMBU_PRINT_ERROR;p++){',
 ' dc_bambu_status_t st={.connected=true,.print_state=p};',
 ' assert(strcmp(dv_bambu_live_state_str(&st),names[p])==0);assert(lighting(st)==lights[p]);',
 ' st.connected=false;st.printing=true;st.error=true;',
 ' dc_bambu_status_t before=st;',
 ' assert(dv_bambu_live_phase(&st)==DC_BAMBU_PRINT_UNKNOWN);assert(strcmp(dv_bambu_live_state_str(&st),"unknown")==0);',
 ' s_pstatus=lighting(st);assert(s_pstatus==DV_PS_NONE);assert(printer_status_color()==s_cfg.idle);',
 ' assert(memcmp(&st,&before,sizeof(st))==0);',
 ' st.connected=true;st.printing=false;st.error=false;assert(lighting(st)==lights[p]);',
 '}',
 'dc_bambu_status_t st={.connected=true,.print_state=DC_BAMBU_PRINT_UNKNOWN,.printing=true};',
 'assert(dv_bambu_live_phase(&st)==DC_BAMBU_PRINT_PRINTING);st.error=true;assert(dv_bambu_live_phase(&st)==DC_BAMBU_PRINT_ERROR);',
 'st.print_state=999;st.error=false;st.printing=false;assert(dv_bambu_live_phase(&st)==DC_BAMBU_PRINT_IDLE);',
 'st.connected=false;assert(dv_bambu_live_phase(&st)==DC_BAMBU_PRINT_UNKNOWN);',
 'return 0;}'
 ].join('\n');
 const dir=mkdtempSync(join(tmpdir(),'bambu-live-map-'));
 try{
  writeFileSync(join(dir,'esp_err.h'),'#pragma once\ntypedef int esp_err_t;\n');
  writeFileSync(join(dir,'test.c'),code);
  execFileSync(process.env.CC||'cc',['-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+dir,'-I'+include,'-I'+join(root,'firmware/components/dv_policy/include'),join(dir,'test.c'),'-o',join(dir,'test')],{stdio:'pipe'});
  execFileSync(join(dir,'test'),{stdio:'pipe'});
 }finally{rmSync(dir,{recursive:true,force:true});}
});
