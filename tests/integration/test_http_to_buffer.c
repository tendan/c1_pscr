#include "unity_fixture.h"
#include "weather/weather_client.h"
#include "weather/weather_transformer.h"
#include "buffer/shared_buffer.h"

#include <pthread.h>

#define FAKE_SERVER_ENDPOINT "http://localhost:8080/data/2.5/weather"
#define FAKE_SERVER_APPID    "test_token"

static struct WeatherClientContext s_weather_ctx;
static SharedBuffer s_buffer;

typedef struct {
    const struct WeatherClientContext *weather_ctx;
    struct WeatherQueryParams params;
    SharedBuffer *buffer;
} ReaderArgs;

static void *integration_reader(void *arg)
{
    ReaderArgs *args = (ReaderArgs *) arg;
    struct RawWeatherData raw = {0};
    struct CalculatedWeatherData calc = {0};

    enum WeatherDataResult result = receive_coordinates_weather_data(
        args->weather_ctx, &args->params, &raw);

    if (result == READ_OK) {
        estimate_irradiance(&raw, &calc);
        shared_buffer_produce(args->buffer, &calc, 1, READ_OK);
    } else {
        shared_buffer_produce(args->buffer, &calc, 0, result);
    }

    return NULL;
}

TEST_GROUP(HttpToBuffer);

TEST_SETUP(HttpToBuffer)
{
    s_weather_ctx = (struct WeatherClientContext){
        .endpoint = FAKE_SERVER_ENDPOINT,
        .appid = FAKE_SERVER_APPID,
        .client_ops = {.fetch = curl_fetch}
    };

    shared_buffer_init(&s_buffer);
}

TEST_TEAR_DOWN(HttpToBuffer)
{
    shared_buffer_destroy(&s_buffer);
}

TEST(HttpToBuffer, SinglePointFetchLandsInBuffer)
{
    ReaderArgs args = {
        .weather_ctx = &s_weather_ctx,
        .params = {
            .latitude = 52.23f,
            .longitude = 21.01f,
            .unit_type = CELCIUS
        },
        .buffer = &s_buffer
    };

    pthread_t tid;
    pthread_create(&tid, NULL, integration_reader, &args);
    pthread_join(tid, NULL);

    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    BufferResult br = shared_buffer_consume(
        &s_buffer, &out, &valid, &status);

    TEST_ASSERT_EQUAL(BUFFER_OK, br);
    TEST_ASSERT_EQUAL_INT(1, valid);
    TEST_ASSERT_EQUAL(READ_OK, status);
}

TEST(HttpToBuffer, FetchedCoordinatesMatchRequest)
{
    ReaderArgs args = {
        .weather_ctx = &s_weather_ctx,
        .params = {
            .latitude = 52.23f,
            .longitude = 21.01f,
            .unit_type = CELCIUS
        },
        .buffer = &s_buffer
    };

    pthread_t tid;
    pthread_create(&tid, NULL, integration_reader, &args);
    pthread_join(tid, NULL);

    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    shared_buffer_consume(&s_buffer, &out, &valid, &status);

    TEST_ASSERT_FLOAT_WITHIN(0.1f, 52.23f, out.latitude);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 21.01f, out.longitude);
}

TEST(HttpToBuffer, InvalidEndpointProducesInvalidBuffer)
{
    struct WeatherClientContext bad_ctx = {
        .endpoint = "http://localhost:9999/nonexistent",
        .appid = FAKE_SERVER_APPID,
        .client_ops = {.fetch = curl_fetch}
    };

    ReaderArgs args = {
        .weather_ctx = &bad_ctx,
        .params = {
            .latitude = 52.23f,
            .longitude = 21.01f,
            .unit_type = CELCIUS
        },
        .buffer = &s_buffer
    };

    pthread_t tid;
    pthread_create(&tid, NULL, integration_reader, &args);
    pthread_join(tid, NULL);

    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    shared_buffer_consume(&s_buffer, &out, &valid, &status);
    TEST_ASSERT_EQUAL_INT(0, valid);
}
