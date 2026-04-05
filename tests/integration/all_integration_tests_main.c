#include "unity_fixture.h"

static void run_all_integration_tests(void)
{
    RUN_TEST_GROUP(HttpToBuffer);
    RUN_TEST_GROUP(BufferToMqtt);
    RUN_TEST_GROUP(PipelineFull);
}

int main(int argc, const char *argv[])
{
    return UnityMain(argc, argv, run_all_integration_tests);
}