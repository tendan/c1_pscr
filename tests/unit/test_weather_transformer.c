#include "unity_fixture.h"
#include "weather/weather_transformer.h"
#include "weather/weather_parser.h"

#include <time.h>

/* July 15th 2024, ~12:00 UTC */
#define SUMMER_NOON_UTC    1721044800L
/* July 15th 2024, 00:00 UTC */
#define SUMMER_MIDNIGHT_UTC 1721001600L
/* Sunny morning, December */
#define WINTER_NOON_UTC    1734004800L

static struct RawWeatherData    s_raw;
static struct CalculatedWeatherData s_out;

TEST_GROUP(WeatherTransformer);

TEST_SETUP(WeatherTransformer)
{
    s_raw = (struct RawWeatherData){
        .temperature = 20.0f,
        .latitude    = 52.0f,
        .longitude   = 21.0f,
        .cloudiness  = 0,
        .unix_time   = SUMMER_NOON_UTC
    };
    s_out = (struct CalculatedWeatherData){0};
}

TEST_TEAR_DOWN(WeatherTransformer)
{

}

TEST(WeatherTransformer, NullRawInputReturnsInvalidInput)
{
    TEST_ASSERT_EQUAL(TRANSFORM_INVALID_INPUT,
        estimate_irradiance(NULL, &s_out));
}

TEST(WeatherTransformer, NullOutputReturnsInvalidInput)
{
    TEST_ASSERT_EQUAL(TRANSFORM_INVALID_INPUT,
        estimate_irradiance(&s_raw, NULL));
}

TEST(WeatherTransformer, CloudinessAbove100ReturnsInvalidInput)
{
    s_raw.cloudiness = 101;
    TEST_ASSERT_EQUAL(TRANSFORM_INVALID_INPUT,
        estimate_irradiance(&s_raw, &s_out));
}

TEST(WeatherTransformer, CloudinessBelow0ReturnsInvalidInput)
{
    s_raw.cloudiness = -1;
    TEST_ASSERT_EQUAL(TRANSFORM_INVALID_INPUT,
        estimate_irradiance(&s_raw, &s_out));
}

TEST(WeatherTransformer, MidnightReturnsZeroIrradiance)
{
    s_raw.unix_time = SUMMER_MIDNIGHT_UTC;
    TEST_ASSERT_EQUAL(TRANSFORM_OK,
        estimate_irradiance(&s_raw, &s_out));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, s_out.irradiance);
}

TEST(WeatherTransformer, ClearSkyNoonIrradianceIsPositive)
{
    s_raw.cloudiness = 0;
    TEST_ASSERT_EQUAL(TRANSFORM_OK,
        estimate_irradiance(&s_raw, &s_out));
    TEST_ASSERT_GREATER_THAN(0.0f, s_out.irradiance);
}

TEST(WeatherTransformer, MoreCloudsReducesIrradiance)
{
    struct CalculatedWeatherData out_clear   = {0};
    struct CalculatedWeatherData out_cloudy  = {0};

    s_raw.cloudiness = 0;
    estimate_irradiance(&s_raw, &out_clear);

    s_raw.cloudiness = 80;
    estimate_irradiance(&s_raw, &out_cloudy);

    TEST_ASSERT_GREATER_THAN(out_cloudy.irradiance, out_clear.irradiance);
}

TEST(WeatherTransformer, FullCloudCoverReducesIrradianceSignificantly)
{
    struct CalculatedWeatherData out_clear  = {0};
    struct CalculatedWeatherData out_full   = {0};

    s_raw.cloudiness = 0;
    estimate_irradiance(&s_raw, &out_clear);

    s_raw.cloudiness = 100;
    estimate_irradiance(&s_raw, &out_full);

    /* Kasten-Czeplak model */
    TEST_ASSERT_LESS_THAN(out_clear.irradiance * 0.30f, out_full.irradiance);
}

TEST(WeatherTransformer, WinterNoonIrradianceLowerThanSummerNoon)
{
    struct CalculatedWeatherData out_summer = {0};
    struct CalculatedWeatherData out_winter = {0};

    s_raw.cloudiness = 0;

    s_raw.unix_time = SUMMER_NOON_UTC;
    estimate_irradiance(&s_raw, &out_summer);

    s_raw.unix_time = WINTER_NOON_UTC;
    estimate_irradiance(&s_raw, &out_winter);

    TEST_ASSERT_GREATER_THAN(out_winter.irradiance, out_summer.irradiance);
}

TEST(WeatherTransformer, OutputContainsInputCoordinates)
{
    TEST_ASSERT_EQUAL(TRANSFORM_OK,
        estimate_irradiance(&s_raw, &s_out));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_raw.latitude,  s_out.latitude);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_raw.longitude, s_out.longitude);
}

TEST(WeatherTransformer, OutputContainsInputTemperature)
{
    TEST_ASSERT_EQUAL(TRANSFORM_OK,
        estimate_irradiance(&s_raw, &s_out));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_raw.temperature, s_out.temperature);
}