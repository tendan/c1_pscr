// tests/unit/grid/test_grid_loader_runner.c
#include "unity_fixture.h"

TEST_GROUP_RUNNER(GridLoader)
{
    RUN_TEST_CASE(GridLoader, NullPathReturnsNullInput);
    RUN_TEST_CASE(GridLoader, NullOutputReturnsNullInput);
    RUN_TEST_CASE(GridLoader, MissingFileReturnsFileNotFound);
    RUN_TEST_CASE(GridLoader, LoadsSinglePointCorrectly);
    RUN_TEST_CASE(GridLoader, LoadsCityNameCorrectly);
    RUN_TEST_CASE(GridLoader, CityNameTruncatedToMaxLength);
    RUN_TEST_CASE(GridLoader, LoadsMultiplePointsCorrectly);
    RUN_TEST_CASE(GridLoader, CommentsAreIgnored);
    RUN_TEST_CASE(GridLoader, EmptyLinesAreIgnored);
    RUN_TEST_CASE(GridLoader, EmptyFileReturnsEmpty);
    RUN_TEST_CASE(GridLoader, OnlyCommentsReturnsEmpty);
    RUN_TEST_CASE(GridLoader, InvalidLineIsSkipped);
    RUN_TEST_CASE(GridLoader, LatitudeOutOfPolandBoundsIsSkipped);
    RUN_TEST_CASE(GridLoader, LongitudeOutOfPolandBoundsIsSkipped);
    RUN_TEST_CASE(GridLoader, FallbackLoadsHardcodedPoints);
    RUN_TEST_CASE(GridLoader, FallbackNullOutputDoesNotCrash);
    RUN_TEST_CASE(GridLoader, FallbackPointsWithinPolandBounds);
}