#include <unity_fixture.h>

static void run_all_tests(void)
{
    RUN_TEST_GROUP(WeatherClient);
    RUN_TEST_GROUP(WeatherClientInternal);
    RUN_TEST_GROUP(WeatherParser);
    RUN_TEST_GROUP(WeatherTransformer);
    RUN_TEST_GROUP(WeatherSerializer);
    RUN_TEST_GROUP(Grid);
    RUN_TEST_GROUP(Config);
    RUN_TEST_GROUP(MqttPublisher);
}

int main(int argc, const char *argv[])
{
    return UnityMain(argc, argv, run_all_tests);
}