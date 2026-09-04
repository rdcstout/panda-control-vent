import assert from 'node:assert/strict';
import test from 'node:test';
import {readFileSync,writeFileSync,mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url));
function block(s,needle){
 const a=s.indexOf(needle); assert.ok(a>=0,needle);
 let i=s.indexOf('{',a),depth=1;
 for(i++;depth&&i<s.length;i++){if(s[i]==='{')depth++;if(s[i]==='}')depth--;}
 assert.equal(depth,0);return s.slice(a,i);
}
test('real MQTT handler and parser restore live phases after offline presentation',()=>{
 const core=readFileSync(join(root,'firmware/components/dc_bambu/dc_bambu.c'),'utf8');
 const main=readFileSync(join(root,'firmware/main/app_main.c'),'utf8');
 let c=readFileSync(new URL('fixtures/bambu_disconnect.c',import.meta.url),'utf8');
 for(const name of ['find_float','expire_completion_locked','parse_report','topic_is_report','mqtt_event_handler','dc_bambu_get_status']){
  const prefix=name==='dc_bambu_get_status'?'esp_err_t ':core.includes('static bool '+name+'(')?'static bool ':'static void ';
  c=c.replace('/* INJECT:'+name+' */',block(core,prefix+name+'('));
 }
 c=c.replace('/* INJECT:lighting_switch */',block(main,'switch (dv_bambu_live_phase(&st))'));
 assert.ok(!c.includes('/* INJECT:'));
 const dir=mkdtempSync(join(tmpdir(),'pcv-disconnect-regression-'));
 try{
  writeFileSync(join(dir,'esp_err.h'),'#pragma once\ntypedef int esp_err_t;\n');
  writeFileSync(join(dir,'test.c'),c);
  execFileSync(process.env.CC||'cc',['-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+dir,'-I'+join(root,'firmware/components/dc_bambu/include'),'-I'+join(root,'firmware/components/dv_policy/include'),join(dir,'test.c'),'-o',join(dir,'test')],{stdio:'pipe'});
  console.log(execFileSync(join(dir,'test'),{encoding:'utf8'}));
 }finally{rmSync(dir,{recursive:true,force:true});}
});
