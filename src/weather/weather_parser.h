//
// Created by tendan on 3.04.2026.
//

#ifndef C1_WEATHER_PARSER_H
#define C1_WEATHER_PARSER_H
#include <stddef.h>
#include <stdint.h>

struct RawWeatherData {
    float temperature;
    float latitude;
    float longitude;
    int8_t cloudiness;
    long unix_time;
};

enum ParseResult {
    PARSE_OK = 0,
    PARSE_MISSING_FIELD = 1,
    PARSE_MALFORMED,
    PARSE_BUFFER_EMPTY
};

enum ParseResult parse_weather_response(const char *response_buf, struct RawWeatherData *out);

#endif //C1_WEATHER_PARSER_H