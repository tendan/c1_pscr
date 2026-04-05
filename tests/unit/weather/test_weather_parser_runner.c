
#include <unity_fixture.h>

TEST_GROUP_RUNNER(WeatherParser)
{
    RUN_TEST_CASE(WeatherParser, ParsesLatitudeCorrectly);
    RUN_TEST_CASE(WeatherParser, ParsesLongitudeCorrectly);
    RUN_TEST_CASE(WeatherParser, ParsesTemperatureCorrectly);
    RUN_TEST_CASE(WeatherParser, ParsesWindCorrectly);
    RUN_TEST_CASE(WeatherParser, ParsesCloudinessCorrectly);
    RUN_TEST_CASE(WeatherParser, MissingFieldReturnsError);
    RUN_TEST_CASE(WeatherParser, MalformedJsonReturnsError);
}