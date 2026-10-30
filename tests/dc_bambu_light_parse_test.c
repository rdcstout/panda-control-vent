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
    assert(dc_bambu_print_error_code(
        "{\"history\":{\"print_error\":77},\"print\":{\"print_error\":12}}",
        &print_error));
    assert(print_error == 12);
    assert(!dc_bambu_print_error_code(
        "{\"print\":{\"history\":{\"print_error\":77}}}", &print_error));
    assert(!dc_bambu_print_error_code(
        "{\"print\":{\"print_error\":4294967296}}", &print_error));
    assert(dc_bambu_next_print_error_code(
        1234, true, DC_BAMBU_GCODE_PRINTING, false, 0) == 0);
    assert(dc_bambu_next_print_error_code(
        1234, true, DC_BAMBU_GCODE_ERROR, false, 0) == 1234);
    assert(dc_bambu_next_print_error_code(
        1234, true, DC_BAMBU_GCODE_ERROR, true, 0) == 0);
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

    char filament[16];
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"0\",\"tray\":["
        "{\"id\":\"0\"},{\"id\":\"1\",\"tray_type\":\"PETG\"}]}],"
        "\"tray_now\":\"1\"}}}", filament, sizeof filament) == DC_FILA_PRESENT);
    assert(strcmp(filament, "PETG") == 0);
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"0\",\"tray\":["
        "{\"id\":\"0\",\"tray_type\":\"PLA\"}]},{\"id\":\"1\",\"tray\":["
        "{\"id\":\"0\"},{\"id\":\"1\",\"tray_type\":\"PETG-CF\"}]}],"
        "\"tray_now\":\"5\"}}}", filament, sizeof filament) == DC_FILA_PRESENT);
    assert(strcmp(filament, "PETG-CF") == 0);
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"0\",\"tray\":[{\"id\":\"2\"}]}],"
        "\"tray_now\":\"2\"}}}", filament, sizeof filament) == DC_FILA_ABSENT);
    assert(dc_bambu_active_filament(
        "{\"print\":{\"bed_temper\":70}}", filament, sizeof filament) == DC_FILA_ABSENT);
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"tray_now\":\"254\"},\"vt_tray\":{"
        "\"id\":\"254\",\"tray_type\":\"TPU\"}}}", filament, sizeof filament)
        == DC_FILA_PRESENT);
    assert(strcmp(filament, "TPU") == 0);
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"0\",\"tray\":["
        "{\"id\":\"0\"},{\"id\":\"1\",\"tray_type\":\"PETG\"}]}],"
        "\"tray_now\":\"1\"},\"bed_temper\":70,\"later_incomplete\":{",
        filament, sizeof filament) == DC_FILA_PRESENT);
    assert(strcmp(filament, "PETG") == 0);
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"128\",\"tray\":["
        "{\"id\":\"0\",\"tray_type\":\"PETG\"}]}],\"tray_now\":\"128\"}}}",
        filament, sizeof filament) == DC_FILA_PRESENT);
    assert(strcmp(filament, "PETG") == 0);
    // ams ht missing type
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"128\",\"tray\":[{\"id\":\"0\"}]}],\"tray_now\":\"128\"}}}", filament, sizeof filament) == DC_FILA_ABSENT);
    // ams ht explicit empty
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"128\",\"tray\":[{\"id\":\"0\",\"tray_type\":\"\"}]}],\"tray_now\":\"128\"}}}", filament, sizeof filament) == DC_FILA_EMPTY);
    // ams ht type-only delta
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"128\",\"tray\":[{\"id\":\"0\",\"tray_type\":\"PETG\"}]}]}}}", filament, sizeof filament) == DC_FILA_ABSENT);
    // ams ht truncated tray
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"128\",\"tray\":[{\"id\":\"0\",\"tray_type\":\"PE", filament, sizeof filament) == DC_FILA_ABSENT);
    // ams explicit empty
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"ams\":[{\"id\":\"0\",\"tray\":[{\"id\":\"2\",\"tray_type\":\"\"}]}],\"tray_now\":\"2\"}}}", filament, sizeof filament) == DC_FILA_EMPTY);
    // explicit no spool
    assert(dc_bambu_active_filament(
        "{\"print\":{\"ams\":{\"tray_now\":\"255\"}}}", filament, sizeof filament) == DC_FILA_EMPTY);
    return 0;
}
