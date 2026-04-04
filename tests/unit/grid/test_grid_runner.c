#include "unity_fixture.h"

TEST_GROUP_RUNNER(Grid)
{
    RUN_TEST_CASE(Grid, PointCountEqualsMinimumRequired);
    RUN_TEST_CASE(Grid, GetPointReturnsNullForOutOfBoundsIndex);
    RUN_TEST_CASE(Grid, GetPointReturnsValidPointerForLastIndex);
    RUN_TEST_CASE(Grid, AllPointsWithinPolandLatitudeBounds);
    RUN_TEST_CASE(Grid, AllPointsWithinPolandLongitudeBounds);
    RUN_TEST_CASE(Grid, NoTwoPointsInSameGridSquare);
    RUN_TEST_CASE(Grid, ExternArrayAndGetPointAreConsistent);
}