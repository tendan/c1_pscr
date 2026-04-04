#include "unity_fixture.h"
#include "weather/weather_client_internal.h"
#include "mock_http.h"

#include <string.h>

static struct WeatherClientContext s_ctx;

TEST_GROUP(WeatherClientInternal);

TEST_SETUP(WeatherClientInternal)
{
    mock_http_reset();
    s_ctx = (struct WeatherClientContext){
        .endpoint   = "https://api.openweathermap.org/data/2.5/weather",
        .appid      = "test_token",
        .client_ops = { .fetch = mock_fetch }
    };
}

TEST_TEAR_DOWN(WeatherClientInternal)
{
    mock_http_reset();
}

TEST(WeatherClientInternal, UrlContainsLatitude)
{
    mock_http_set_result(HTTP_OK);
    mock_http_set_response("{}");

    char buf[4096] = {0};
    const struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };

    perform_http_request(&s_ctx, &params, buf, sizeof(buf));

    TEST_ASSERT_NOT_NULL(strstr(mock_http_get_last_url(), "lat="));
}

TEST(WeatherClientInternal, UrlContainsLongitude)
{
    mock_http_set_result(HTTP_OK);
    mock_http_set_response("{}");

    char buf[4096] = {0};
    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };

    perform_http_request(&s_ctx, &params, buf, sizeof(buf));

    TEST_ASSERT_NOT_NULL(strstr(mock_http_get_last_url(), "lon="));
}

TEST(WeatherClientInternal, UrlContainsAppid)
{
    mock_http_set_result(HTTP_OK);
    mock_http_set_response("{}");

    char buf[4096] = {0};
    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };

    perform_http_request(&s_ctx, &params, buf, sizeof(buf));

    TEST_ASSERT_NOT_NULL(strstr(mock_http_get_last_url(), "test_token"));
}

TEST(WeatherClientInternal, BufferIsNullTerminated)
{
    mock_http_set_result(HTTP_OK);
    mock_http_set_response("{\"main\":{\"temp\":21.5}}");

    char buf[4096];
    memset(buf, 0xFF, sizeof(buf));

    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };

    perform_http_request(&s_ctx, &params, buf, sizeof(buf));

    TEST_ASSERT_EQUAL_CHAR('\0', buf[strlen(buf)]);
}

TEST(WeatherClientInternal, CelciusUnitTypeProducesMetricInUrl)
{
    mock_http_set_result(HTTP_OK);
    mock_http_set_response("{}");

    char buf[4096] = {0};
    struct WeatherQueryParams params = {
        .latitude  = 52.2f,
        .longitude = 21.0f,
        .unit_type = CELCIUS
    };

    perform_http_request(&s_ctx, &params, buf, sizeof(buf));

    TEST_ASSERT_NOT_NULL(strstr(mock_http_get_last_url(), "units=metric"));
}