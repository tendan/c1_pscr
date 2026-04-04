//
// Created by tendan on 31.03.2026.
//

#include "weather_client.h"

enum HttpResult perform_http_request(float latitude, float longitude, enum UnitType units)
{
    return HTTP_OK;
}

enum WeatherDataResult receive_coordinates_weather_data(const struct WeatherClientContext *ctx,
                                                        struct WeatherQueryParams *params,
                                                        struct RawWeatherData *raw_weather_data)
{
    return READ_OK;
}
