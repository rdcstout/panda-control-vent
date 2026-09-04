#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "dc_bambu_parse.h"
#include "dv_bambu_state.h"
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_ERR_INVALID_ARG 1
#define portMAX_DELAY 0
#define xSemaphoreTake(a,b) ((void)0)
#define xSemaphoreGive(a) ((void)0)
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGD(...) ((void)0)
#define RX_CAP 16384
#define COMPLETION_HOLD_US 7500000LL
typedef const char *esp_event_base_t;
typedef enum { MQTT_EVENT_CONNECTED, MQTT_EVENT_DISCONNECTED, MQTT_EVENT_DATA } esp_mqtt_event_id_t;
typedef struct {int current_data_offset,topic_len,data_len,total_data_len;const char *topic,*data;} mqtt_event_t;
typedef mqtt_event_t *esp_mqtt_event_handle_t;
#define esp_mqtt_client_subscribe(...) ((void)0)
#define esp_mqtt_client_publish(...) ((void)0)
static int s_lock=1;
static dc_bambu_status_t s_status;
static int64_t clock_us=1000000;
static int64_t esp_timer_get_time(void){return clock_us;}
static char s_report_topic[80]="device/test/report";
static char rx_buffer[RX_CAP], *s_rx=rx_buffer;
static size_t s_rx_len;
static bool s_in_report;
static int64_t s_chamber_temp_us;
static dc_bambu_gcode_phase_t s_gcode_phase;
static uint32_t s_print_error_code;
static bool s_has_observed_active_job,s_completion_visible;
static int64_t s_completion_until_us;
/* INJECT:find_float */

/* INJECT:expire_completion_locked */

/* INJECT:parse_report */

/* INJECT:topic_is_report */

/* INJECT:mqtt_event_handler */

/* INJECT:dc_bambu_get_status */
typedef enum { DV_PS_NONE,DV_PS_IDLE,DV_PS_PREPARING,DV_PS_PRINTING,DV_PS_PAUSED,DV_PS_COMPLETE,DV_PS_ERROR } dv_printer_status_t;
static dv_printer_status_t lighting(dc_bambu_status_t st){
 dv_printer_status_t status=DV_PS_NONE;
 /* INJECT:lighting_switch */
 return status;
}

static void report(const char *json){
 mqtt_event_t e={.topic=s_report_topic,.topic_len=(int)strlen(s_report_topic),.data=json,.data_len=(int)strlen(json),.total_data_len=(int)strlen(json)};
 mqtt_event_handler(NULL,NULL,MQTT_EVENT_DATA,&e);
}
static void reset_fixture(void){
 memset(&s_status,0,sizeof(s_status));s_gcode_phase=DC_BAMBU_GCODE_UNKNOWN;
 s_print_error_code=0;s_has_observed_active_job=false;s_completion_visible=false;s_completion_until_us=0;
 s_chamber_temp_us=0;clock_us=1000000;s_in_report=false;s_rx_len=0;
}
static void show(const char *label){
 dc_bambu_status_t st;assert(dc_bambu_get_status(&st)==ESP_OK);
 printf("%s: connected=%s api=%s lighting_enum=%d\n",label,st.connected?"true":"false",dv_bambu_live_state_str(&st),lighting(st));
}
int main(void){
 const char *phases[]={"PAUSE","RUNNING","IDLE","FAILED"};
 const dc_bambu_print_state_t expected[]={DC_BAMBU_PRINT_PAUSED,DC_BAMBU_PRINT_PRINTING,DC_BAMBU_PRINT_IDLE,DC_BAMBU_PRINT_ERROR};

 for(int i=0;i<4;i++){
  reset_fixture();char json[256];
  snprintf(json,sizeof(json),"{\"print\":{\"bed_temper\":25,\"gcode_state\":\"%s\",\"print_error\":%d}}",phases[i],i==3?1234:0);
  report(json);assert(s_status.connected && s_status.print_state==expected[i]);show(phases[i]);
  mqtt_event_handler(NULL,NULL,MQTT_EVENT_DISCONNECTED,NULL);
  clock_us+=60000000;
  dc_bambu_status_t st;assert(dc_bambu_get_status(&st)==ESP_OK);
  assert(!st.connected && st.print_state==expected[i] && lighting(st)==DV_PS_NONE && dv_bambu_live_phase(&st)==DC_BAMBU_PRINT_UNKNOWN);show("after disconnect +60 seconds");
  mqtt_event_handler(NULL,NULL,MQTT_EVENT_CONNECTED,NULL);
  assert(!s_status.connected);show("socket reconnected, no report yet");
  report("{\"print\":{\"bed_temper\":25,\"gcode_state\":\"RUNNING\",\"print_error\":0}}");
  assert(s_status.connected && s_status.print_state==DC_BAMBU_PRINT_PRINTING && lighting(s_status)==DV_PS_PRINTING);
  show("fresh RUNNING report");
 }
 reset_fixture();
 report("{\"print\":{\"bed_temper\":25,\"gcode_state\":\"RUNNING\",\"print_error\":0}}");
 report("{\"print\":{\"bed_temper\":25,\"gcode_state\":\"FINISH\",\"print_error\":0}}");
 assert(s_status.print_state==DC_BAMBU_PRINT_COMPLETE);
 mqtt_event_handler(NULL,NULL,MQTT_EVENT_DISCONNECTED,NULL);
 clock_us+=7499999;dc_bambu_status_t st;dc_bambu_get_status(&st);assert(st.print_state==DC_BAMBU_PRINT_COMPLETE);
 clock_us+=1;dc_bambu_get_status(&st);assert(!st.connected && st.print_state==DC_BAMBU_PRINT_IDLE);
 show("COMPLETE expires at 7.5s while disconnected");
 puts("PASS: disconnected phases masked; fresh-report recovery works; completion timer unchanged.");
}
