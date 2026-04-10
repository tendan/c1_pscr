#include "unity_fixture.h"

#include "main_helpers.h"

#define EXISTING_FILE  PROJECT_SOURCE_DIR "/deploy/c1_app.conf.example"
#define MISSING_FILE   "/nonexistent/grid.csv"

TEST_GROUP(MainHelpers);

TEST_SETUP(MainHelpers)
{

}

TEST_TEAR_DOWN(MainHelpers)
{

}

TEST(MainHelpers, CliPathTakesPriorityOverDefault)
{
    const char *result = resolve_grid_path(
        "/tmp/cli.csv",
        "/etc/c1/grid.csv"
    );
    TEST_ASSERT_EQUAL_STRING("/tmp/cli.csv", result);
}

TEST(MainHelpers, NullCliPathFallsBackToDefault)
{
    const char *result = resolve_grid_path(
        NULL,
        EXISTING_FILE
    );
    TEST_ASSERT_EQUAL_STRING(EXISTING_FILE, result);
}

TEST(MainHelpers, NullCliPathAndMissingDefaultReturnsNull)
{
    const char *result = resolve_grid_path(
        NULL,
        MISSING_FILE
    );
    TEST_ASSERT_NULL(result);
}