#include "dc_bambu_parse.h"

#include <assert.h>

int main(void)
{
    assert(dc_bambu_gcode_phase("RUNNING") == DC_BAMBU_GCODE_PRINTING);
    assert(dc_bambu_gcode_phase("PAUSE") == DC_BAMBU_GCODE_PAUSED);
    assert(dc_bambu_gcode_phase("FINISH") == DC_BAMBU_GCODE_COMPLETE);
    uint32_t print_error = 99;
    assert(dc_bambu_print_error_code("{\"print\":{\"print_error\":0}}", &print_error));
    assert(print_error == 0);
    assert(dc_bambu_print_error_code("{\"print\":{\"print_error\":\"0x04D2\"}}", &print_error));
    assert(print_error == 1234);
    assert(dc_bambu_chamber_light("{\"print\":{}}") == DC_BAMBU_LIGHT_ABSENT);
    assert(dc_bambu_chamber_light(
        "{\"print\":{\"lights_report\":[{\"mode\":\"on\",\"node\":\"chamber_light\"}]}}")
        == DC_BAMBU_LIGHT_ON);
    assert(dc_bambu_chamber_light(
        "{\"print\":{\"lights_report\":[{\"node\":\"chamber_light\",\"mode\":\"off\"}]}}")
        == DC_BAMBU_LIGHT_OFF);
    assert(dc_bambu_chamber_light(
        "{\"print\":{\"lights_report\":[{\"mode\":\"flashing\",\"node\":\"chamber_light\"}]}}")
        == DC_BAMBU_LIGHT_ON);
    assert(dc_bambu_chamber_light(
        "{\"print\":{\"lights_report\":[{\"mode\":\"on\",\"node\":\"work_light\"}]}}")
        == DC_BAMBU_LIGHT_ABSENT);
    return 0;
}
