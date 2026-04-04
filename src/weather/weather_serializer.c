#include "weather/weather_serializer.h"
#include "cjson/cJSON.h"

#include <string.h>

static double round_to_2dp(double val)
{
    return (double) ((int) (val * 100.0 + 0.5)) / 100.0;
}

SerializeResult serialize_weather_data(
    const struct CalculatedWeatherData *in,
    char *buf,
    size_t buf_len)
{
    if (in == NULL || buf == NULL) {
        return SERIALIZE_ERR_NULL_INPUT;
    }

    if (buf_len < WEATHER_SERIALIZER_BUFFER_SIZE) {
        return SERIALIZE_ERR_BUFFER_TOO_SMALL;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return SERIALIZE_ERR_INTERNAL;
    }

    /* Wszystkie floaty z dokładnością 2 miejsc po przecinku */
    if (!cJSON_AddNumberToObject(root, "lat", round_to_2dp((double) in->latitude)) ||
        !cJSON_AddNumberToObject(root, "lon", round_to_2dp((double) in->longitude)) ||
        !cJSON_AddNumberToObject(root, "temperature", round_to_2dp((double) in->temperature)) ||
        !cJSON_AddNumberToObject(root, "irradiance", round_to_2dp((double) in->irradiance)) ||
        !cJSON_AddNumberToObject(root, "wind_speed", round_to_2dp((double) in->wind_speed)) ||
        !cJSON_AddNumberToObject(root, "wind_degrees", round_to_2dp((double) in->wind_degrees))
    ) {
        cJSON_Delete(root);
        return SERIALIZE_ERR_INTERNAL;
    }

    /* Formatowanie z 2 miejscami po przecinku */
    cJSON_bool print_result = cJSON_PrintPreallocated(root, buf, (int) buf_len, 0);
    cJSON_Delete(root);

    if (!print_result) {
        return SERIALIZE_ERR_BUFFER_TOO_SMALL;
    }

    return SERIALIZE_OK;
}
