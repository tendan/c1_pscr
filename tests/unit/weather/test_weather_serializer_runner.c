
#include "unity_fixture.h"

TEST_GROUP_RUNNER(WeatherSerializer)
{
    RUN_TEST_CASE(WeatherSerializer, NullInputReturnsNullInput);
    RUN_TEST_CASE(WeatherSerializer, NullBufferReturnsNullInput);
    RUN_TEST_CASE(WeatherSerializer, SmallBufferReturnsBufferTooSmall);
    RUN_TEST_CASE(WeatherSerializer, HappyPathReturnsSerializeOk);
    RUN_TEST_CASE(WeatherSerializer, OutputIsValidJson);
    RUN_TEST_CASE(WeatherSerializer, OutputContainsLatField);
    RUN_TEST_CASE(WeatherSerializer, OutputContainsCorrectTemperature);
    RUN_TEST_CASE(WeatherSerializer, OutputContainsCorrectWindDegrees);
    RUN_TEST_CASE(WeatherSerializer, OutputBufferIsNullTerminated);
}