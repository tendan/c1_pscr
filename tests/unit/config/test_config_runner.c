#include "unity_fixture.h"

TEST_GROUP_RUNNER(Config)
{
    RUN_TEST_CASE(Config, LoadsHostCorrectly);
    RUN_TEST_CASE(Config, LoadsPortCorrectly);
    RUN_TEST_CASE(Config, LoadsTopicPrefixCorrectly);
    RUN_TEST_CASE(Config, CommentsAndEmptyLinesAreIgnored);
    RUN_TEST_CASE(Config, InvalidPortReturnsInvalidValue);
    RUN_TEST_CASE(Config, MissingHostReturnsMissingKey);
    RUN_TEST_CASE(Config, LoadsAppidCorrectly);
    RUN_TEST_CASE(Config, MissingAppidReturnsMissingKey);
}