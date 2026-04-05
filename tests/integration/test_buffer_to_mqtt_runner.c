#include "unity_fixture.h"

TEST_GROUP_RUNNER(BufferToMqtt)
{
    RUN_TEST_CASE(BufferToMqtt, ValidDataIsPublished);
    RUN_TEST_CASE(BufferToMqtt, InvalidDataIsNotPublished);
}