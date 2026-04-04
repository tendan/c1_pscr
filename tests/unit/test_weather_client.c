#include "unity_fixture.h"
#include "weather/weather_client.h"
#include "mock_http.h"

static struct WeatherClientContext s_ctx;

TEST_GROUP(WeatherClient);

TEST_SETUP(WeatherClient)
{
    mock_http_reset();
    s_ctx = (struct WeatherClientContext){
        .endpoint   = "https://api.openweathermap.org/data/2.5/weather",
        .appid      = "test_token",
        .client_ops = { .fetch = mock_fetch }
    };
}

TEST_TEAR_DOWN(WeatherClient)
{
    mock_http_reset();
}

TEST(WeatherClient, HappyPathReturnsReadOk)
{
    mock_http_set_response(
        "{\"coord\":{\"lat\":52.2,\"lon\":21.0},"
        "\"main\":{\"temp\":21.5},"
        "\"clouds\":{\"all\":20},"
        "\"dt\":1700000000}"
    );
    mock_http_set_result(HTTP_OK);

    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };
    struct RawWeatherData out = {0};

    TEST_ASSERT_EQUAL(READ_OK,
        receive_coordinates_weather_data(&s_ctx, &params, &out));
}

TEST(WeatherClient, TimeoutReturnsReadTimeout)
{
    mock_http_set_result(HTTP_TIMEOUT);

    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };
    struct RawWeatherData out = {0};

    TEST_ASSERT_EQUAL(READ_TIMEOUT,
        receive_coordinates_weather_data(&s_ctx, &params, &out));
}

TEST(WeatherClient, ForbiddenReturnsReadForbidden)
{
    mock_http_set_result(HTTP_FORBIDDEN);

    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };
    struct RawWeatherData out = {0};

    TEST_ASSERT_EQUAL(READ_FORBIDDEN,
        receive_coordinates_weather_data(&s_ctx, &params, &out));
}

TEST(WeatherClient, FetchCalledExactlyOnce)
{
    mock_http_set_response(
        "{\"coord\":{\"lat\":52.2,\"lon\":21.0},"
        "\"main\":{\"temp\":21.5},"
        "\"clouds\":{\"all\":20},"
        "\"dt\":1700000000}"
    );
    mock_http_set_result(HTTP_OK);

    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };
    struct RawWeatherData out = {0};

    receive_coordinates_weather_data(&s_ctx, &params, &out);

    TEST_ASSERT_EQUAL_INT(1, mock_http_get_call_count());
}