
#include <unity_fixture.h>

TEST_GROUP_RUNNER(WeatherClient)
{
    RUN_TEST_CASE(WeatherClient, HappyPathReturnsReadOk);
    RUN_TEST_CASE(WeatherClient, UrlContainsLatAndLon);
    RUN_TEST_CASE(WeatherClient, TimeoutPropagatesAsReadTimeout);
    RUN_TEST_CASE(WeatherClient, FetchCalledExactlyOnce);
    RUN_TEST_CASE(WeatherClient, BufferIsNullTerminated);
    RUN_TEST_CASE(WeatherClient, UrlBuiltCorrectly);
}