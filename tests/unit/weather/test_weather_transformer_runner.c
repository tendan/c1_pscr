#include "unity_fixture.h"

TEST_GROUP_RUNNER(WeatherTransformer)
{
    RUN_TEST_CASE(WeatherTransformer, NullRawInputReturnsInvalidInput);
    RUN_TEST_CASE(WeatherTransformer, NullOutputReturnsInvalidInput);
    RUN_TEST_CASE(WeatherTransformer, CloudinessAbove100ReturnsInvalidInput);
    RUN_TEST_CASE(WeatherTransformer, CloudinessBelow0ReturnsInvalidInput);
    RUN_TEST_CASE(WeatherTransformer, MidnightReturnsZeroIrradiance);
    RUN_TEST_CASE(WeatherTransformer, ClearSkyNoonIrradianceIsPositive);
    RUN_TEST_CASE(WeatherTransformer, MoreCloudsReducesIrradiance);
    RUN_TEST_CASE(WeatherTransformer, FullCloudCoverReducesIrradianceSignificantly);
    RUN_TEST_CASE(WeatherTransformer, WinterNoonIrradianceLowerThanSummerNoon);
    RUN_TEST_CASE(WeatherTransformer, OutputContainsInputCoordinates);
    RUN_TEST_CASE(WeatherTransformer, OutputContainsInputTemperature);
}