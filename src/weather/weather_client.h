//
// Created by tendan on 31.03.2026.
//

#ifndef C1_WEATHER_CLIENT_H
#define C1_WEATHER_CLIENT_H
#include <stddef.h>

#include "weather_parser.h"

// Enums

enum HttpResult {
    HTTP_OK = 0,
    HTTP_BAD_REQUEST = 1,
    HTTP_FORBIDDEN,
    HTTP_NOT_FOUND,
    HTTP_INTERNAL_SERVER_ERROR,
    HTTP_TIMEOUT
};

enum WeatherDataResult {
    READ_OK = 0,
    READ_TIMEOUT = 1,
    READ_FORBIDDEN,
    READ_OTHER_ERROR,
};

enum UnitType {
    KELVIN = 0,
    CELCIUS = 1,
    FAHRENHEIT,
};

typedef enum HttpResult (*http_fetch_fn)(
    const char  *url,
    char        *buf,
    size_t       buf_len
);

// Structs

typedef struct {
    http_fetch_fn fetch;
} HttpClientOps;

struct WeatherClientContext {
    char* endpoint;
    char* appid;
    HttpClientOps client_ops;
};

struct WeatherQueryParams {
    float latitude;
    float longitude;
    enum UnitType unit_type;
};

enum WeatherDataResult receive_coordinates_weather_data(const struct WeatherClientContext *ctx, struct WeatherQueryParams *params, struct RawWeatherData *raw_weather_data);


#endif //C1_WEATHER_CLIENT_H