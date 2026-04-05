#include "unity_fixture.h"
#include "buffer/shared_buffer.h"

#include <pthread.h>
#include <string.h>

static SharedBuffer s_buf;

static const struct CalculatedWeatherData s_data = {
    .latitude = 52.23f,
    .longitude = 21.01f,
    .temperature = 20.50f,
    .irradiance = 650.30f,
    .wind_speed = 3.20f,
    .wind_degrees = 270
};

TEST_GROUP(SharedBuffer);

TEST_SETUP(SharedBuffer)
{
    shared_buffer_init(&s_buf);
}

TEST_TEAR_DOWN(SharedBuffer)
{
    shared_buffer_destroy(&s_buf);
}

TEST(SharedBuffer, InitNullReturnsNullInput)
{
    TEST_ASSERT_EQUAL(BUFFER_ERR_NULL_INPUT,
                      shared_buffer_init(NULL));
}

TEST(SharedBuffer, InitReturnsOk)
{
    SharedBuffer buf;
    TEST_ASSERT_EQUAL(BUFFER_OK, shared_buffer_init(&buf));
    shared_buffer_destroy(&buf);
}

TEST(SharedBuffer, ProduceNullBufferReturnsNullInput)
{
    TEST_ASSERT_EQUAL(BUFFER_ERR_NULL_INPUT,
                      shared_buffer_produce(NULL, &s_data, 1, READ_OK));
}

TEST(SharedBuffer, ProduceNullDataReturnsNullInput)
{
    TEST_ASSERT_EQUAL(BUFFER_ERR_NULL_INPUT,
                      shared_buffer_produce(&s_buf, NULL, 1, READ_OK));
}

TEST(SharedBuffer, ConsumeNullBufferReturnsNullInput)
{
    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    TEST_ASSERT_EQUAL(BUFFER_ERR_NULL_INPUT,
                      shared_buffer_consume(NULL, &out, &valid, &status));
}

TEST(SharedBuffer, ProduceReturnsOk)
{
    TEST_ASSERT_EQUAL(BUFFER_OK,
                      shared_buffer_produce(&s_buf, &s_data, 1, READ_OK));
}

TEST(SharedBuffer, ConsumeAfterProduceReturnsOk)
{
    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    shared_buffer_produce(&s_buf, &s_data, 1, READ_OK);
    TEST_ASSERT_EQUAL(BUFFER_OK,
                      shared_buffer_consume(&s_buf, &out, &valid, &status));
}

TEST(SharedBuffer, ConsumedDataMatchesProducedData)
{
    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    shared_buffer_produce(&s_buf, &s_data, 1, READ_OK);
    shared_buffer_consume(&s_buf, &out, &valid, &status);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_data.latitude, out.latitude);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_data.longitude, out.longitude);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_data.temperature, out.temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_data.irradiance, out.irradiance);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, s_data.wind_speed, out.wind_speed);
    TEST_ASSERT_EQUAL_INT(s_data.wind_degrees, out.wind_degrees);
}

TEST(SharedBuffer, ConsumedValidFlagMatchesProduced)
{
    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    shared_buffer_produce(&s_buf, &s_data, 1, READ_OK);
    shared_buffer_consume(&s_buf, &out, &valid, &status);

    TEST_ASSERT_EQUAL_INT(1, valid);
}

TEST(SharedBuffer, ConsumedStatusMatchesProduced)
{
    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OTHER_ERROR;

    shared_buffer_produce(&s_buf, &s_data, 0, READ_OTHER_ERROR);
    shared_buffer_consume(&s_buf, &out, &valid, &status);

    TEST_ASSERT_EQUAL_INT(0, valid);
    TEST_ASSERT_EQUAL(READ_OTHER_ERROR, status);
}

/* ── Timeout ───────────────────────────────────────────────── */

TEST(SharedBuffer, ConsumeWithoutProduceReturnsTimeout)
{
    struct CalculatedWeatherData out = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    TEST_ASSERT_EQUAL(BUFFER_ERR_TIMEOUT,
                      shared_buffer_consume(&s_buf, &out, &valid, &status));
}

/* ── Walidacja wejścia ─────────────────────────────────────── */

TEST(SharedBuffer, ConsumeNullOutputReturnsNullInput)
{
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    TEST_ASSERT_EQUAL(BUFFER_ERR_NULL_INPUT,
                      shared_buffer_consume(&s_buf, NULL, &valid, &status));
}

TEST(SharedBuffer, ConsumeNullValidReturnsNullInput)
{
    struct CalculatedWeatherData out = {0};
    enum WeatherDataResult status = READ_OK;

    TEST_ASSERT_EQUAL(BUFFER_ERR_NULL_INPUT,
                      shared_buffer_consume(&s_buf, &out, NULL, &status));
}

TEST(SharedBuffer, ConsumeNullStatusReturnsNullInput)
{
    struct CalculatedWeatherData out = {0};
    int valid = 0;

    TEST_ASSERT_EQUAL(BUFFER_ERR_NULL_INPUT,
                      shared_buffer_consume(&s_buf, &out, &valid, NULL));
}

typedef struct {
    SharedBuffer *buf;
    struct CalculatedWeatherData data;
    BufferResult result;
} ThreadArgs;

static void *producer_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *) arg;
    args->result = shared_buffer_produce(
        args->buf, &args->data, 1, READ_OK);
    return NULL;
}

static void *consumer_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *) arg;
    int valid = 0;
    enum WeatherDataResult status = READ_OK;
    args->result = shared_buffer_consume(
        args->buf, &args->data, &valid, &status);
    return NULL;
}

TEST(SharedBuffer, ProducerConsumerInSeparateThreads)
{
    pthread_t prod_tid, cons_tid;
    ThreadArgs prod_args = {.buf = &s_buf, .data = s_data};
    ThreadArgs cons_args = {.buf = &s_buf};

    pthread_create(&cons_tid, NULL, consumer_thread, &cons_args);
    pthread_create(&prod_tid, NULL, producer_thread, &prod_args);

    pthread_join(prod_tid, NULL);
    pthread_join(cons_tid, NULL);

    TEST_ASSERT_EQUAL(BUFFER_OK, prod_args.result);
    TEST_ASSERT_EQUAL(BUFFER_OK, cons_args.result);
    TEST_ASSERT_FLOAT_WITHIN(0.001f,
                             s_data.temperature, cons_args.data.temperature);
}

TEST(SharedBuffer, ProducerBlocksUntilConsumed)
{
    pthread_t prod1_tid, prod2_tid, cons_tid;
    ThreadArgs prod1_args = {.buf = &s_buf, .data = s_data};
    ThreadArgs prod2_args = {.buf = &s_buf, .data = s_data};
    ThreadArgs cons_args = {.buf = &s_buf};

    pthread_create(&prod1_tid, NULL, producer_thread, &prod1_args);
    pthread_join(prod1_tid, NULL);

    pthread_create(&prod2_tid, NULL, producer_thread, &prod2_args);
    pthread_create(&cons_tid, NULL, consumer_thread, &cons_args);

    pthread_join(prod2_tid, NULL);
    pthread_join(cons_tid, NULL);

    TEST_ASSERT_EQUAL(BUFFER_OK, prod2_args.result);
    TEST_ASSERT_EQUAL(BUFFER_OK, cons_args.result);
}
