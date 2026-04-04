//
// Created by tendan on 3.04.2026.
//

#include "weather_parser.h"

#include <string.h>

#include "cjson/cJSON.h"

enum ParseResult parse_weather_response(const char *response_buf, struct RawWeatherData *out)
{
    if (response_buf == NULL || response_buf[0] == '\0') {
        return PARSE_BUFFER_EMPTY;
    }

    const char* error = {0};
    cJSON *full_json = cJSON_ParseWithOpts(response_buf, &error, 1);
    if (full_json == NULL) {
        return PARSE_MALFORMED;
    }

    cJSON *num_values;
    enum ParseResult result = PARSE_OK;

    const cJSON *coord_part = cJSON_GetObjectItemCaseSensitive(full_json, "coord");
    if (!cJSON_IsObject(coord_part)) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }

    num_values = cJSON_GetObjectItemCaseSensitive(coord_part, "lat");
    if (!cJSON_IsNumber(num_values) || !num_values->valuedouble) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }
    out->latitude = (float)num_values->valuedouble;

    num_values = cJSON_GetObjectItemCaseSensitive(coord_part, "lon");
    if (!cJSON_IsNumber(num_values)) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }
    out->longitude = (float)num_values->valuedouble;

    const cJSON *main_part = cJSON_GetObjectItemCaseSensitive(full_json, "main");
    if (!cJSON_IsObject(main_part) || !cJSON_HasObjectItem(main_part, "temp")) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }
    num_values = cJSON_GetObjectItemCaseSensitive(main_part, "temp");
    if (!cJSON_IsNumber(num_values)) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }
    out->temperature = (float)num_values->valuedouble;

    const cJSON *clouds_part = cJSON_GetObjectItemCaseSensitive(full_json, "clouds");

    if (!cJSON_IsObject(clouds_part) || !cJSON_HasObjectItem(clouds_part, "all")) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }
    num_values = cJSON_GetObjectItemCaseSensitive(clouds_part, "all");
    if (!cJSON_IsNumber(num_values)) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }
    out->cloudiness = (int8_t)num_values->valueint;

    num_values = cJSON_GetObjectItem(full_json, "dt");
    if (!cJSON_IsNumber(num_values)) {
        result = PARSE_MISSING_FIELD;
        goto parse_end;
    }
    out->unix_time = num_values->valueint;


parse_end:
    cJSON_Delete(full_json);
    return result;
}