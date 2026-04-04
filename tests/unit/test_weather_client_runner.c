
#include <unity_fixture.h>

TEST_GROUP_RUNNER(WeatherClient)
{
    RUN_TEST_CASE(WeatherClient, HappyPathReturnsReadOk);
    RUN_TEST_CASE(WeatherClient, TimeoutReturnsReadTimeout);
    RUN_TEST_CASE(WeatherClient, ForbiddenReturnsReadForbidden);
    RUN_TEST_CASE(WeatherClient, FetchCalledExactlyOnce);
}

TEST_GROUP_RUNNER(WeatherClientInternal)
{
    RUN_TEST_CASE(WeatherClientInternal, UrlContainsLatitude);
    RUN_TEST_CASE(WeatherClientInternal, UrlContainsLongitude);
    RUN_TEST_CASE(WeatherClientInternal, UrlContainsAppid);
    RUN_TEST_CASE(WeatherClientInternal, BufferIsNullTerminated);
    RUN_TEST_CASE(WeatherClientInternal, CelciusUnitTypeProducesMetricInUrl);
}