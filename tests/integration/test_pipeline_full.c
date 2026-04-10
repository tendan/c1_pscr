#include "unity_fixture.h"
#include "pipeline/pipeline.h"
#include "weather/weather_client.h"
#include "mqtt/mqtt_publisher.h"
#include "grid/grid.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FAKE_SERVER_ENDPOINT "http://localhost:8080/data/2.5/weather"
#define FAKE_SERVER_APPID    "test_token"
#define TEST_BROKER_HOST     "localhost"
#define TEST_BROKER_PORT     1884
#define TEST_TOPIC_PREFIX    "agh/kse/c1/weather"

static struct WeatherClientContext s_weather_ctx;
static struct MqttPublisherContext s_mqtt_ctx;
static struct AppConfig s_config;
static PipelineContext s_pipeline_ctx;

TEST_GROUP(PipelineFull);

TEST_SETUP(PipelineFull)
{
    s_weather_ctx = (struct WeatherClientContext){
        .endpoint = FAKE_SERVER_ENDPOINT,
        .appid = FAKE_SERVER_APPID,
        .client_ops = {.fetch = curl_fetch}
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
        .mqtt_ctx = &s_mqtt_ctx,
        .mqtt_handle = NULL,
    };

    pipeline_init(&s_pipeline_ctx);

    /* Wyczyść retained messages ze wszystkich topiców siatki */
    for (size_t i = 0; i < grid_point_count(); i++) {
        const struct GridPoint *p = grid_get_fallback_point(i);
        char clear_cmd[256];
        snprintf(clear_cmd, sizeof(clear_cmd),
            "mosquitto_pub -h localhost -p 1884"
            " -t 'agh/kse/c1/weather/%.2f/%.2f'"
            " -r -n 2>/dev/null",
            (double)p->latitude,
            (double)p->longitude);
        system(clear_cmd);
    }

    usleep(200000);
}

TEST_TEAR_DOWN(PipelineFull)
{
    pipeline_cleanup(&s_pipeline_ctx);
}

TEST(PipelineFull, AllGridPointsPublished)
{
    FILE *sub = popen(
        "mosquitto_sub -h localhost -p 1884"
        " -t 'agh/kse/c1/weather/#'"
        " -C 10 -W 30 2>/dev/null",
        "r"
    );
    TEST_ASSERT_NOT_NULL(sub);

    /* Czekaj aż subskrybent się połączy z brokerem */
    usleep(300000); /* 300ms */

    PipelineResult result = pipeline_run(&s_pipeline_ctx);
    TEST_ASSERT_EQUAL(PIPELINE_OK, result);

    int msg_count = 0;
    char line[512];
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
        " -v" /* ← verbose: wypisuje topic + payload */
        " -C 10 -W 30 2>/dev/null",
        "r"
    );
    TEST_ASSERT_NOT_NULL(sub);

    usleep(300000);

    pipeline_run(&s_pipeline_ctx);

    int found_warszawa = 0;
    char line[512];
    while (fgets(line, sizeof(line), sub) != NULL) {
        /* Linia wygląda teraz: "agh/kse/c1/weather/52.23/21.01 {json...}" */
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

    int all_valid = 1;
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
