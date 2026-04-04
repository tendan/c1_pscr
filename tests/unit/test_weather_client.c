//
// Created by tendan on 3.04.2026.
//

#include <unity_fixture.h>

#include <string.h>

#include "weather/weather_client.h"
#include "weather/weather_client_internal.h"
#include "weather/weather_parser.h"
#include "mock_http.h"

static struct WeatherClientContext s_ctx;

TEST_GROUP(WeatherClient);

TEST_SETUP(WeatherClient)
{
    mock_http_reset();
    s_ctx = (struct WeatherClientContext){
        .endpoint = "http://localhost:8080",
        .appid = "demo_appid",
        .client_ops = { .fetch = mock_fetch }
    };
}

TEST_TEAR_DOWN(WeatherClient)
{
    mock_http_reset();
}

TEST(WeatherClient, HappyPathReturnsReadOk)
{
    mock_http_set_response("{\"main\":{\"temp\":21.5},\"clouds\":{\"all\":20}}");
    mock_http_set_result(HTTP_OK);

    struct WeatherQueryParams params = { .latitude = 52.2f, .longitude = 21.0f };
    struct RawWeatherData out = {0};
    enum WeatherDataResult result =
        receive_coordinates_weather_data(&s_ctx, &params, &out);

    TEST_ASSERT_EQUAL(READ_OK, result);
}

TEST(WeatherClient, UrlContainsLatAndLon)
{
    mock_http_set_response("{\"main\":{\"temp\":21.5},\"clouds\":{\"all\":20}}");
    mock_http_set_result(HTTP_OK);

    struct WeatherQueryParams params = { .latitude = 52.2f, .longitude = 21.0f };
    struct RawWeatherData out = {0};
    receive_coordinates_weather_data(&s_ctx, &params, &out);

    /* Weryfikujesz kontrakt — czy URL zawiera współrzędne */
    TEST_ASSERT_NOT_NULL(strstr(mock_http_get_last_url(), "lat=52.2"));
    TEST_ASSERT_NOT_NULL(strstr(mock_http_get_last_url(), "lon=21.0"));
}

TEST(WeatherClient, TimeoutPropagatesAsReadTimeout)
{
    mock_http_set_result(HTTP_TIMEOUT);

    struct WeatherQueryParams params = { .latitude = 52.2f, .longitude = 21.0f };
    struct RawWeatherData out = {0};
    enum WeatherDataResult result =
        receive_coordinates_weather_data(&s_ctx, &params, &out);

    TEST_ASSERT_EQUAL(READ_TIMEOUT, result);
}

TEST(WeatherClient, FetchCalledExactlyOnce)
{
    mock_http_set_response("{\"main\":{\"temp\":21.5},\"clouds\":{\"all\":20}}");
    mock_http_set_result(HTTP_OK);

    struct WeatherQueryParams params = { .latitude = 52.2f, .longitude = 21.0f };
    struct RawWeatherData out = {0};
    receive_coordinates_weather_data(&s_ctx, &params, &out);

    TEST_ASSERT_EQUAL_INT(1, mock_http_get_call_count());
}

TEST(WeatherClient, BufferIsNullTerminated)
{
    TEST_FAIL();
}

TEST(WeatherClient, UrlBuiltCorrectly)
{
    TEST_FAIL();
}