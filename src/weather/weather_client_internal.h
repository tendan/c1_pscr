
#ifndef C1_WEATHER_CLIENT_INTERNAL_H
#define C1_WEATHER_CLIENT_INTERNAL_H

#include "weather_client.h"


enum HttpResult perform_http_request(
    const struct WeatherClientContext *ctx,
    const struct WeatherQueryParams   *params,
    char                              *response_buf,
    size_t                             buf_len
);

#endif //C1_WEATHER_CLIENT_INTERNAL_H