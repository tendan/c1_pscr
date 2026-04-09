
#include "unity_fixture.h"
#include "weather/weather_serializer.h"
#include "weather/weather_transformer.h"

#include <cjson/cJSON.h>
#include <string.h>

static struct CalculatedWeatherData s_data;
static char s_buf[WEATHER_SERIALIZER_BUFFER_SIZE];

TEST_GROUP(WeatherSerializer);

TEST_SETUP(WeatherSerializer)
{
    s_data = (struct CalculatedWeatherData){
        .latitude     = 52.23f,
        .longitude    = 21.01f,
        .temperature  = 20.50f,
        .irradiance   = 650.30f,
        .wind_speed   = 3.20f,
        .wind_degrees = 270
    };
    memset(s_buf, 0, sizeof(s_buf));
}

TEST_TEAR_DOWN(WeatherSerializer) {}

TEST(WeatherSerializer, NullInputReturnsNullInput)
{
    TEST_ASSERT_EQUAL(SERIALIZE_ERR_NULL_INPUT,
        serialize_weather_data(NULL, s_buf, sizeof(s_buf)));
}

TEST(WeatherSerializer, NullBufferReturnsNullInput)
{
    TEST_ASSERT_EQUAL(SERIALIZE_ERR_NULL_INPUT,
        serialize_weather_data(&s_data, NULL, sizeof(s_buf)));
}

TEST(WeatherSerializer, SmallBufferReturnsBufferTooSmall)
{
    char small_buf[16];
    TEST_ASSERT_EQUAL(SERIALIZE_ERR_BUFFER_TOO_SMALL,
        serialize_weather_data(&s_data, small_buf, sizeof(small_buf)));
}

TEST(WeatherSerializer, HappyPathReturnsSerializeOk)
{
    TEST_ASSERT_EQUAL(SERIALIZE_OK,
        serialize_weather_data(&s_data, s_buf, sizeof(s_buf)));
}

TEST(WeatherSerializer, OutputIsValidJson)
{
    serialize_weather_data(&s_data, s_buf, sizeof(s_buf));

    cJSON *parsed = cJSON_Parse(s_buf);
    TEST_ASSERT_NOT_NULL(parsed);
    cJSON_Delete(parsed);
}

TEST(WeatherSerializer, OutputContainsLatField)
{
    serialize_weather_data(&s_data, s_buf, sizeof(s_buf));

    cJSON *parsed = cJSON_Parse(s_buf);
    TEST_ASSERT_NOT_NULL(cJSON_GetObjectItem(parsed, "lat"));
    cJSON_Delete(parsed);
}

TEST(WeatherSerializer, OutputContainsCorrectTemperature)
{
    serialize_weather_data(&s_data, s_buf, sizeof(s_buf));

    cJSON *parsed = cJSON_Parse(s_buf);
    cJSON *temp   = cJSON_GetObjectItem(parsed, "temperature");
    TEST_ASSERT_NOT_NULL(temp);
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 20.50, temp->valuedouble);
    cJSON_Delete(parsed);
}

TEST(WeatherSerializer, OutputContainsCorrectWindDegrees)
{
    serialize_weather_data(&s_data, s_buf, sizeof(s_buf));

    cJSON *parsed  = cJSON_Parse(s_buf);
    cJSON *degrees = cJSON_GetObjectItem(parsed, "wind_degrees");
    TEST_ASSERT_NOT_NULL(degrees);
    TEST_ASSERT_EQUAL_INT(270, degrees->valueint);
    cJSON_Delete(parsed);
}

TEST(WeatherSerializer, OutputBufferIsNullTerminated)
{
    serialize_weather_data(&s_data, s_buf, sizeof(s_buf));
    TEST_ASSERT_EQUAL_CHAR('\0', s_buf[strlen(s_buf)]);
}