#include "unity_fixture.h"
#include "grid/grid_loader.h"

#include <stdio.h>
#include <string.h>

#define TEST_CSV_PATH "/tmp/test_grid.csv"

static void write_csv(const char *contents)
{
    FILE *f = fopen(TEST_CSV_PATH, "w");
    fputs(contents, f);
    fclose(f);
}

static GridPointArray s_out;

TEST_GROUP(GridLoader);

TEST_SETUP(GridLoader)
{
    memset(&s_out, 0, sizeof(s_out));
}

TEST_TEAR_DOWN(GridLoader)
{
    remove(TEST_CSV_PATH);
}

TEST(GridLoader, NullPathReturnsNullInput)
{
    TEST_ASSERT_EQUAL(GRID_LOAD_ERR_NULL_INPUT,
        grid_load_from_file(NULL, &s_out));
}

TEST(GridLoader, NullOutputReturnsNullInput)
{
    TEST_ASSERT_EQUAL(GRID_LOAD_ERR_NULL_INPUT,
        grid_load_from_file(TEST_CSV_PATH, NULL));
}

TEST(GridLoader, MissingFileReturnsFileNotFound)
{
    TEST_ASSERT_EQUAL(GRID_LOAD_ERR_FILE_NOT_FOUND,
        grid_load_from_file("/tmp/nonexistent_grid.csv", &s_out));
}

TEST(GridLoader, LoadsSinglePointCorrectly)
{
    write_csv("Warszawa,52.23,21.01\n");

    TEST_ASSERT_EQUAL(GRID_LOAD_OK,
        grid_load_from_file(TEST_CSV_PATH, &s_out));
    TEST_ASSERT_EQUAL_size_t(1, s_out.count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 52.23f, s_out.points[0].latitude);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.01f, s_out.points[0].longitude);
}

TEST(GridLoader, LoadsCityNameCorrectly)
{
    write_csv("Warszawa,52.23,21.01\n");

    grid_load_from_file(TEST_CSV_PATH, &s_out);

    TEST_ASSERT_EQUAL_STRING("Warszawa", s_out.points[0].city_name);
}

TEST(GridLoader, LoadsMultiplePointsCorrectly)
{
    write_csv(
        "Warszawa,52.23,21.01\n"
        "Krakow,50.06,19.94\n"
        "Gdansk,54.35,18.64\n"
    );

    TEST_ASSERT_EQUAL(GRID_LOAD_OK,
        grid_load_from_file(TEST_CSV_PATH, &s_out));
    TEST_ASSERT_EQUAL_size_t(3, s_out.count);
}

TEST(GridLoader, CommentsAreIgnored)
{
    write_csv(
        "# To jest komentarz\n"
        "Warszawa,52.23,21.01\n"
    );

    grid_load_from_file(TEST_CSV_PATH, &s_out);
    TEST_ASSERT_EQUAL_size_t(1, s_out.count);
}

TEST(GridLoader, EmptyLinesAreIgnored)
{
    write_csv(
        "\n"
        "Warszawa,52.23,21.01\n"
        "\n"
    );

    grid_load_from_file(TEST_CSV_PATH, &s_out);
    TEST_ASSERT_EQUAL_size_t(1, s_out.count);
}

TEST(GridLoader, EmptyFileReturnsEmpty)
{
    write_csv("");

    TEST_ASSERT_EQUAL(GRID_LOAD_ERR_EMPTY,
        grid_load_from_file(TEST_CSV_PATH, &s_out));
}

TEST(GridLoader, OnlyCommentsReturnsEmpty)
{
    write_csv(
        "# komentarz 1\n"
        "# komentarz 2\n"
    );

    TEST_ASSERT_EQUAL(GRID_LOAD_ERR_EMPTY,
        grid_load_from_file(TEST_CSV_PATH, &s_out));
}

TEST(GridLoader, InvalidLineIsSkipped)
{
    write_csv(
        "Warszawa,52.23,21.01\n"
        "NIEPRAWIDLOWA_LINIA\n"
        "Krakow,50.06,19.94\n"
    );

    grid_load_from_file(TEST_CSV_PATH, &s_out);
    TEST_ASSERT_EQUAL_size_t(2, s_out.count);
}

TEST(GridLoader, LatitudeOutOfPolandBoundsIsSkipped)
{
    write_csv(
        "Warszawa,52.23,21.01\n"
        "OutOfBounds,48.00,21.01\n"
    );

    grid_load_from_file(TEST_CSV_PATH, &s_out);
    TEST_ASSERT_EQUAL_size_t(1, s_out.count);
}

TEST(GridLoader, LongitudeOutOfPolandBoundsIsSkipped)
{
    write_csv(
        "Warszawa,52.23,21.01\n"
        "OutOfBounds,52.23,25.00\n"
    );

    grid_load_from_file(TEST_CSV_PATH, &s_out);
    TEST_ASSERT_EQUAL_size_t(1, s_out.count);
}

TEST(GridLoader, FallbackLoadsHardcodedPoints)
{
    grid_load_fallback(&s_out);

    TEST_ASSERT_GREATER_OR_EQUAL(10U, s_out.count);
}

TEST(GridLoader, FallbackNullOutputDoesNotCrash)
{
    grid_load_fallback(NULL);
}

TEST(GridLoader, FallbackPointsWithinPolandBounds)
{
    grid_load_fallback(&s_out);

    for (size_t i = 0; i < s_out.count; i++) {
        TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(49.0f,
            s_out.points[i].latitude);
        TEST_ASSERT_LESS_OR_EQUAL_FLOAT(54.9f,
            s_out.points[i].latitude);
    }
}