#include "unity_fixture.h"

TEST_GROUP_RUNNER(PipelineFull)
{
    RUN_TEST_CASE(PipelineFull, AllGridPointsPublished);
    RUN_TEST_CASE(PipelineFull, PublishedTopicsContainCoordinates);
    RUN_TEST_CASE(PipelineFull, PublishedPayloadsAreValidJson);
}