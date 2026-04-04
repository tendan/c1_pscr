//
// Created by tendan on 31.03.2026.
//

#ifndef C1_WEATHER_CLIENT_H
#define C1_WEATHER_CLIENT_H

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

// Structs

struct HttpClientOps {

};

struct WeatherClientContext {
    char* endpoint;
    char* appid;
    struct HttpClientOps client_ops;
};

struct WeatherQueryParams {
    float latitude;
    float longitude;
    enum UnitType unit_type;
};

enum WeatherDataResult receive_coordinates_weather_data(const struct WeatherClientContext *ctx, struct WeatherQueryParams params, struct RawWeatherData *raw_weather_data);


#endif //C1_WEATHER_CLIENT_H