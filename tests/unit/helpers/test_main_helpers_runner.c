#include "unity_fixture.h"

TEST_GROUP_RUNNER(MainHelper)
{
    RUN_TEST_CASE(MainHelpers, CliPathTakesPriorityOverDefault);
    RUN_TEST_CASE(MainHelpers, NullCliPathFallsBackToDefault);
    RUN_TEST_CASE(MainHelpers, NullCliPathAndMissingDefaultReturnsNull);
}