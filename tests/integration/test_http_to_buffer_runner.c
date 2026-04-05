#include "unity_fixture.h"

TEST_GROUP_RUNNER(HttpToBuffer)
{
    RUN_TEST_CASE(HttpToBuffer, SinglePointFetchLandsInBuffer);
    RUN_TEST_CASE(HttpToBuffer, FetchedCoordinatesMatchRequest);
    RUN_TEST_CASE(HttpToBuffer, InvalidEndpointProducesInvalidBuffer);
}