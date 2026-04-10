#include "unity_fixture.h"
#include "pipeline/pipeline.h"
#include "mock_http.h"
#include "mock_mqtt.h"

#include <string.h>

static struct WeatherClientContext s_weather_ctx;
static struct MqttPublisherContext s_mqtt_ctx;
static struct AppConfig s_config;
static PipelineContext s_pipeline_ctx;
static GridPointArray s_grid;

static const char *s_valid_response =
        "{\"coord\":{\"lat\":52.23,\"lon\":21.01},"
        "\"main\":{\"temp\":20.5,\"feels_like\":19.0,"
        "\"temp_min\":18.0,\"temp_max\":22.0,"
        "\"pressure\":1013,\"humidity\":60},"
        "\"wind\":{\"speed\":3.2,\"deg\":270},"
        "\"clouds\":{\"all\":20},"
        "\"dt\":1721044800}";

TEST_GROUP(Pipeline);

TEST_SETUP(Pipeline)
{
    mock_http_reset();
    mock_mqtt_reset();

    grid_load_fallback(&s_grid);

    s_weather_ctx = (struct WeatherClientContext){
        .endpoint = "https://api.openweathermap.org/data/2.5/weather",
        .appid = "test_token",
        .client_ops = {.fetch = mock_fetch}
    };

    strncpy(s_config.mqtt_host, "localhost", CONFIG_HOST_MAX_LEN - 1);
    strncpy(s_config.mqtt_topic_prefix,
            "agh/kse/c1/weather", CONFIG_TOPIC_MAX_LEN - 1);
    s_config.mqtt_port = 1883;

    mqtt_publisher_context_from_config(
        &s_config, &MOCK_MQTT_OPS, &s_mqtt_ctx);

    s_pipeline_ctx = (PipelineContext){
        .weather_ctx = &s_weather_ctx,
        .mqtt_ctx = &s_mqtt_ctx,
        .mqtt_handle = NULL,
        .grid_point_array = &s_grid,
    };
}

TEST_TEAR_DOWN(Pipeline)
{
    mock_http_reset();
    mock_mqtt_reset();
}

TEST(Pipeline, InitNullContextReturnsNullInput)
{
    TEST_ASSERT_EQUAL(PIPELINE_ERR_NULL_INPUT,
                      pipeline_init(NULL));
}

TEST(Pipeline, InitReturnsOkWithValidContext)
{
    TEST_ASSERT_EQUAL(PIPELINE_OK,
                      pipeline_init(&s_pipeline_ctx));
    pipeline_cleanup(&s_pipeline_ctx);
}

TEST(Pipeline, InitSetsClientHandle)
{
    pipeline_init(&s_pipeline_ctx);
    TEST_ASSERT_NOT_NULL(s_pipeline_ctx.mqtt_handle);
    pipeline_cleanup(&s_pipeline_ctx);
}

TEST(Pipeline, InitConnectCalledOnce)
{
    pipeline_init(&s_pipeline_ctx);
    TEST_ASSERT_EQUAL_INT(1, mock_mqtt_get_connect_call_count());
    pipeline_cleanup(&s_pipeline_ctx);
}

TEST(Pipeline, CleanupNullContextDoesNotCrash)
{
    pipeline_cleanup(NULL);
}

TEST(Pipeline, CleanupDisconnectCalledOnce)
{
    pipeline_init(&s_pipeline_ctx);
    pipeline_cleanup(&s_pipeline_ctx);
    TEST_ASSERT_EQUAL_INT(1, mock_mqtt_get_disconnect_call_count());
}

TEST(Pipeline, CleanupNullsClientHandle)
{
    pipeline_init(&s_pipeline_ctx);
    pipeline_cleanup(&s_pipeline_ctx);
    TEST_ASSERT_NULL(s_pipeline_ctx.mqtt_handle);
}

TEST(Pipeline, RunNullContextReturnsNullInput)
{
    TEST_ASSERT_EQUAL(PIPELINE_ERR_NULL_INPUT,
                      pipeline_run(NULL));
}

TEST(Pipeline, RunPublishesForAllGridPoints)
{
    mock_http_set_response(s_valid_response);
    mock_http_set_result(HTTP_OK);

    pipeline_init(&s_pipeline_ctx);
    pipeline_run(&s_pipeline_ctx);

    int expected_count = (int)s_pipeline_ctx.grid_point_array->count;

    pipeline_cleanup(&s_pipeline_ctx);

    TEST_ASSERT_EQUAL_INT(
        expected_count,
        mock_mqtt_get_publish_call_count()
    );
}

TEST(Pipeline, RunFetchCalledForAllGridPoints)
{
    mock_http_set_response(s_valid_response);
    mock_http_set_result(HTTP_OK);

    pipeline_init(&s_pipeline_ctx);
    pipeline_run(&s_pipeline_ctx);

    int expected_count = (int)s_pipeline_ctx.grid_point_array->count;

    pipeline_cleanup(&s_pipeline_ctx);

    TEST_ASSERT_EQUAL_INT(expected_count,
                          mock_http_get_call_count());
}

TEST(Pipeline, RunWithForbiddenSkipsPublish)
{
    mock_http_set_result(HTTP_FORBIDDEN);

    pipeline_init(&s_pipeline_ctx);
    pipeline_run(&s_pipeline_ctx);
    pipeline_cleanup(&s_pipeline_ctx);

    TEST_ASSERT_EQUAL_INT(0, mock_mqtt_get_publish_call_count());
}

TEST(Pipeline, RunWithTimeoutRetriesAndSkipsPublish)
{
    mock_http_set_result(HTTP_TIMEOUT);

    pipeline_init(&s_pipeline_ctx);
    pipeline_run(&s_pipeline_ctx);

    int expected_count = (int)s_pipeline_ctx.grid_point_array->count;

    pipeline_cleanup(&s_pipeline_ctx);


    /* Każdy wątek próbuje PIPELINE_MAX_RETRIES razy */
    TEST_ASSERT_EQUAL_INT(
        expected_count * PIPELINE_MAX_RETRIES,
        mock_http_get_call_count());
    TEST_ASSERT_EQUAL_INT(0, mock_mqtt_get_publish_call_count());
}
