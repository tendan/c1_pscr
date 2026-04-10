
#include "unity_fixture.h"
#include "grid/grid.h"

TEST_GROUP(Grid);

TEST_SETUP(Grid)
{

}
TEST_TEAR_DOWN(Grid)
{

}

TEST(Grid, PointCountEqualsMinimumRequired)
{
    TEST_ASSERT_GREATER_OR_EQUAL(10U, grid_point_count());
}

TEST(Grid, GetPointReturnsNullForOutOfBoundsIndex)
{
    TEST_ASSERT_NULL(grid_get_fallback_point(GRID_FALLBACK_POINTS_COUNT));
}

TEST(Grid, GetPointReturnsValidPointerForLastIndex)
{
    TEST_ASSERT_NOT_NULL(grid_get_fallback_point(GRID_FALLBACK_POINTS_COUNT - 1));
}

TEST(Grid, AllPointsWithinPolandLatitudeBounds)
{
    for (size_t i = 0; i < grid_point_count(); i++) {
        const struct GridPoint *p = grid_get_fallback_point(i);
        TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(49.0f, p->latitude);
        TEST_ASSERT_LESS_OR_EQUAL_FLOAT(54.9f,    p->latitude);
    }
}

TEST(Grid, AllPointsWithinPolandLongitudeBounds)
{
    for (size_t i = 0; i < grid_point_count(); i++) {
        const struct GridPoint *p = grid_get_fallback_point(i);
        TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(14.1f, p->longitude);
        TEST_ASSERT_LESS_OR_EQUAL_FLOAT(24.2f,    p->longitude);
    }
}

TEST(Grid, NoTwoPointsInSameGridSquare)
{
    /* Tolerancja mniejsza niż rozmiar kwadratu ~0.45° lat x 0.72° lon */
    const float LAT_TOLERANCE = 0.44f;
    const float LON_TOLERANCE = 0.71f;

    for (size_t i = 0; i < grid_point_count(); i++) {
        for (size_t j = i + 1; j < grid_point_count(); j++) {
            const struct GridPoint *a = grid_get_fallback_point(i);
            const struct GridPoint *b = grid_get_fallback_point(j);

            float dlat = a->latitude  - b->latitude;
            float dlon = a->longitude - b->longitude;
            if (dlat < 0.0f) dlat = -dlat;
            if (dlon < 0.0f) dlon = -dlon;

            /* Punkty muszą być w różnych kwadratach */
            int same_lat_band = dlat < LAT_TOLERANCE;
            int same_lon_band = dlon < LON_TOLERANCE;
            TEST_ASSERT_FALSE(same_lat_band && same_lon_band);
        }
    }
}

TEST(Grid, ExternArrayAndGetPointAreConsistent)
{
    for (size_t i = 0; i < grid_point_count(); i++) {
        const struct GridPoint *via_fn  = grid_get_fallback_point(i);
        const struct GridPoint *via_arr = &GRID_FALLBACK_POINTS[i];
        TEST_ASSERT_EQUAL_PTR(via_arr, via_fn);
    }
}