//
// Created by tendan on 4.04.2026.
//

#ifndef C1_WEATHER_SERIALIZER_H
#define C1_WEATHER_SERIALIZER_H

#include <stddef.h>
#include "weather_transformer.h"

#define WEATHER_SERIALIZER_BUFFER_SIZE 256U

typedef enum {
    SERIALIZE_OK = 0,
    SERIALIZE_ERR_NULL_INPUT,
    SERIALIZE_ERR_BUFFER_TOO_SMALL,
    SERIALIZE_ERR_INTERNAL,
} SerializeResult;

SerializeResult serialize_weather_data(
    const struct CalculatedWeatherData *in,
    char                               *buf,
    size_t                              buf_len
);

#endif