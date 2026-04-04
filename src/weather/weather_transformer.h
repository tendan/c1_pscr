//
// Created by tendan on 3.04.2026.
//

#ifndef C1_WEATHER_TRANSFORMER_H
#define C1_WEATHER_TRANSFORMER_H

#include "weather_client.h"
#include "weather_parser.h"

struct CalculatedWeatherData {
    float temperature;
    float latitude;
    float longitude;
    float irradiance;
};

enum TransformResult {
    TRANSFORM_OK = 0,
    TRANSFORM_INVALID_INPUT = 1,
    TRANSFORM_OUT_OF_RANGE
};

enum TransformResult estimate_irradiance(const struct RawWeatherData *raw_weather_data, struct CalculatedWeatherData *parsed_weather_data);

#endif //C1_WEATHER_TRANSFORMER_H