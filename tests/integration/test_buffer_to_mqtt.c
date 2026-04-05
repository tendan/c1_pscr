#include "unity_fixture.h"
#include "buffer/shared_buffer.h"
#include "mqtt/mqtt_publisher.h"

#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define TEST_BROKER_HOST "localhost"
#define TEST_BROKER_PORT 1884
#define TEST_TOPIC_PREFIX "agh/kse/c1/weather"

static SharedBuffer s_buffer;
static struct MqttPublisherContext s_mqtt_ctx;
static struct AppConfig s_config;
static void *s_mqtt_handle;

static const struct CalculatedWeatherData s_data = {
    .latitude = 52.23f,
    .longitude = 21.01f,
    .temperature = 20.50f,
    .irradiance = 650.30f,
    .wind_speed = 3.20f,
    .wind_degrees = 270
};

typedef struct {
    struct MqttPublisherContext *mqtt_ctx;
    void *mqtt_handle;
    SharedBuffer *buffer;
} SenderArgs;

static void *integration_sender(void *arg)
{
    SenderArgs *args = (SenderArgs *) arg;
    struct CalculatedWeatherData data = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    BufferResult br = shared_buffer_consume(
        args->buffer, &data, &valid, &status);

    if (br != BUFFER_OK || !valid) {
        return NULL;
    }

    mqtt_publisher_publish(args->mqtt_ctx, args->mqtt_handle, &data);
    return NULL;
}

TEST_GROUP(BufferToMqtt);

TEST_SETUP(BufferToMqtt)
{
    shared_buffer_init(&s_buffer);
    mqtt_publisher_lib_init();

    strncpy(s_config.mqtt_host,
        TEST_BROKER_HOST, CONFIG_HOST_MAX_LEN - 1);
    snprintf(s_config.mqtt_topic_prefix,
        CONFIG_TOPIC_MAX_LEN, "%s", TEST_TOPIC_PREFIX);
    s_config.mqtt_port = TEST_BROKER_PORT;

    mqtt_publisher_context_from_config(
        &s_config, &MOSQUITTO_OPS, &s_mqtt_ctx);
    mqtt_publisher_connect(&s_mqtt_ctx, &s_mqtt_handle);

    /* Wyczyść dokładny topic używany w teście ValidDataIsPublished */
    system(
        "mosquitto_pub -h localhost -p 1884"
        " -t 'agh/kse/c1/weather/52.23/21.01'"
        " -r -n 2>/dev/null"
    );
    usleep(200000);
}

TEST_TEAR_DOWN(BufferToMqtt)
{
    mqtt_publisher_disconnect(&s_mqtt_ctx, s_mqtt_handle);
    mqtt_publisher_lib_cleanup();
    shared_buffer_destroy(&s_buffer);
}

TEST(BufferToMqtt, ValidDataIsPublished)
{
    /* Subskrybent musi być gotowy PRZED publikacją */
    /* mosquitto_sub z retain lub użyj pipe */
    SenderArgs args = {
        .mqtt_ctx = &s_mqtt_ctx,
        .mqtt_handle = s_mqtt_handle,
        .buffer = &s_buffer
    };

    /* Uruchom subskrybenta w tle przez pipe */
    FILE *sub = popen(
        "mosquitto_sub -h localhost -p 1884"
        " -t 'agh/kse/c1/weather/#' -C 1 -W 5 2>/dev/null",
        "r"
    );
    TEST_ASSERT_NOT_NULL(sub);

    /* Małe opóźnienie żeby subskrybent zdążył się połączyć */
    usleep(200000); /* 200ms */

    shared_buffer_produce(&s_buffer, &s_data, 1, READ_OK);

    pthread_t tid;
    pthread_create(&tid, NULL, integration_sender, &args);
    pthread_join(tid, NULL);

    /* Czytaj wynik */
    char line[512] = {0};
    fgets(line, sizeof(line), sub);
    int rc = pclose(sub);

    TEST_ASSERT_GREATER_THAN(0, strlen(line));
    (void) rc;
}

TEST(BufferToMqtt, InvalidDataIsNotPublished)
{
    SenderArgs args = {
        .mqtt_ctx = &s_mqtt_ctx,
        .mqtt_handle = s_mqtt_handle,
        .buffer = &s_buffer
    };

    shared_buffer_produce(&s_buffer, &s_data, 0, READ_OTHER_ERROR);

    pthread_t tid;
    pthread_create(&tid, NULL, integration_sender, &args);
    pthread_join(tid, NULL);

    int rc = system(
        "mosquitto_sub -h localhost -p 1884"
        " -t 'agh/kse/c1/weather/#' -C 1 -W 2 > /dev/null 2>&1"
    );
    TEST_ASSERT_NOT_EQUAL(0, rc);
}
