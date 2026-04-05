#include "unity_fixture.h"
#include "pipeline/pipeline.h"
#include "weather/weather_client.h"
#include "mqtt/mqtt_publisher.h"
#include "grid/grid.h"

#include <stdio.h>
#include <string.h>

#define FAKE_SERVER_ENDPOINT "http://localhost:8080/data/2.5/weather"
#define FAKE_SERVER_APPID    "test_token"
#define TEST_BROKER_HOST     "localhost"
#define TEST_BROKER_PORT     1884
#define TEST_TOPIC_PREFIX    "agh/kse/c1/weather"

static struct WeatherClientContext s_weather_ctx;
static struct MqttPublisherContext s_mqtt_ctx;
static struct AppConfig            s_config;
static PipelineContext             s_pipeline_ctx;

TEST_GROUP(PipelineFull);

TEST_SETUP(PipelineFull)
{
    s_weather_ctx = (struct WeatherClientContext){
        .endpoint   = FAKE_SERVER_ENDPOINT,
        .appid      = FAKE_SERVER_APPID,
        .client_ops = { .fetch = curl_fetch }
    };

    strncpy(s_config.mqtt_host,
        TEST_BROKER_HOST, CONFIG_HOST_MAX_LEN - 1);
    snprintf(s_config.mqtt_topic_prefix,
        CONFIG_TOPIC_MAX_LEN, "%s", TEST_TOPIC_PREFIX);
    s_config.mqtt_port = TEST_BROKER_PORT;

    mqtt_publisher_context_from_config(
        &s_config, &MOSQUITTO_OPS, &s_mqtt_ctx);

    s_pipeline_ctx = (PipelineContext){
        .weather_ctx = &s_weather_ctx,
        .mqtt_ctx    = &s_mqtt_ctx,
        .mqtt_handle = NULL,
    };

    pipeline_init(&s_pipeline_ctx);
}

TEST_TEAR_DOWN(PipelineFull)
{
    pipeline_cleanup(&s_pipeline_ctx);
}

TEST(PipelineFull, AllGridPointsPublished)
{
    /* Subskrybuj przed uruchomieniem pipeline */
    FILE *sub = popen(
        "mosquitto_sub -h localhost -p 1884"
        " -t 'agh/kse/c1/weather/#'"
        " -C 10 -W 30 2>/dev/null",
        "r"
    );
    TEST_ASSERT_NOT_NULL(sub);

    PipelineResult result = pipeline_run(&s_pipeline_ctx);
    TEST_ASSERT_EQUAL(PIPELINE_OK, result);

    /* Zlicz odebrane wiadomości */
    int   msg_count = 0;
    char  line[512];
    while (fgets(line, sizeof(line), sub) != NULL) {
        msg_count++;
    }
    pclose(sub);

    TEST_ASSERT_EQUAL_INT(GRID_POINT_COUNT, msg_count);
}

TEST(PipelineFull, PublishedTopicsContainCoordinates)
{
    FILE *sub = popen(
        "mosquitto_sub -h localhost -p 1884"
        " -t 'agh/kse/c1/weather/#'"
        " -C 10 -W 30 2>/dev/null",
        "r"
    );
    TEST_ASSERT_NOT_NULL(sub);

    pipeline_run(&s_pipeline_ctx);

    int  found_warszawa = 0;
    char line[512];
    while (fgets(line, sizeof(line), sub) != NULL) {
        if (strstr(line, "52.23") && strstr(line, "21.01")) {
            found_warszawa = 1;
        }
    }
    pclose(sub);

    TEST_ASSERT_EQUAL_INT(1, found_warszawa);
}

TEST(PipelineFull, PublishedPayloadsAreValidJson)
{
    FILE *sub = popen(
        "mosquitto_sub -h localhost -p 1884"
        " -t 'agh/kse/c1/weather/#'"
        " -C 10 -W 30 2>/dev/null",
        "r"
    );
    TEST_ASSERT_NOT_NULL(sub);

    pipeline_run(&s_pipeline_ctx);

    int  all_valid = 1;
    char line[512];
    while (fgets(line, sizeof(line), sub) != NULL) {
        /* Każdy payload musi zaczynać się od '{' */
        char *json_start = strchr(line, '{');
        if (json_start == NULL) {
            all_valid = 0;
        }
    }
    pclose(sub);

    TEST_ASSERT_EQUAL_INT(1, all_valid);
}