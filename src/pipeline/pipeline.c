#include "pipeline/pipeline.h"
#include "weather/weather_transformer.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


static void *reader_thread(void *arg)
{
    PipelineTaskContext *ctx = (PipelineTaskContext *) arg;

    struct RawWeatherData raw = {0};
    struct CalculatedWeatherData calc = {0};
    enum WeatherDataResult result = READ_OK;

    for (int attempt = 0; attempt < PIPELINE_MAX_RETRIES; attempt++) {
        result = receive_coordinates_weather_data(
            ctx->pipeline_ctx->weather_ctx,
            &ctx->params,
            &raw);

        if (result == READ_OK) {
            break;
        }

        if (result == READ_FORBIDDEN) {
            fprintf(stderr,
                    "[thread %d] Fatal error: token forbidden, aborting\n",
                    ctx->thread_id);
            shared_buffer_produce(&ctx->buffer, &calc, 0, READ_FORBIDDEN);
            return NULL;
        }

        fprintf(stderr,
                "[thread %d] Timeout, retry %d/%d\n",
                ctx->thread_id, attempt + 1, PIPELINE_MAX_RETRIES);

        sleep(1);
    }

    if (result != READ_OK) {
        fprintf(stderr,
                "[thread %d] All retries exhausted\n",
                ctx->thread_id);
        shared_buffer_produce(&ctx->buffer, &calc, 0, result);
        return NULL;
    }

    enum TransformResult tr = estimate_irradiance(&raw, &calc);
    if (tr != TRANSFORM_OK) {
        fprintf(stderr,
                "[thread %d] Transform failed\n",
                ctx->thread_id);
        shared_buffer_produce(&ctx->buffer, &calc, 0, READ_OTHER_ERROR);
        return NULL;
    }

    shared_buffer_produce(&ctx->buffer, &calc, 1, READ_OK);
    return NULL;
}

static void *sender_thread(void *arg)
{
    PipelineTaskContext *ctx = (PipelineTaskContext *) arg;
    struct CalculatedWeatherData data = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    BufferResult br = shared_buffer_consume(
        &ctx->buffer, &data, &valid, &status);

    if (br != BUFFER_OK) {
        fprintf(stderr,
                "[thread %d] Buffer consume failed: %d\n",
                ctx->thread_id, br);
        return NULL;
    }

    if (!valid) {
        fprintf(stderr,
                "[thread %d] Data invalid (status=%d), skipping publish\n",
                ctx->thread_id, status);
        return NULL;
    }

    MqttResult mr = mqtt_publisher_publish(
        ctx->pipeline_ctx->mqtt_ctx,
        ctx->pipeline_ctx->mqtt_handle,
        &data);

    if (mr != MQTT_OK) {
        fprintf(stderr,
                "[thread %d] Publish failed: %d\n",
                ctx->thread_id, mr);
    }

    return NULL;
}

PipelineResult pipeline_init(PipelineContext *ctx)
{
    if (ctx == NULL) {
        return PIPELINE_ERR_NULL_INPUT;
    }

    MqttResult mr = mqtt_publisher_lib_init();
    if (mr != MQTT_OK) {
        return PIPELINE_ERR_INIT_FAILED;
    }

    mr = mqtt_publisher_connect(ctx->mqtt_ctx, &ctx->mqtt_handle);
    if (mr != MQTT_OK) {
        mqtt_publisher_lib_cleanup();
        return PIPELINE_ERR_INIT_FAILED;
    }

    return PIPELINE_OK;
}

PipelineResult pipeline_run(const PipelineContext *ctx)
{
    if (ctx == NULL) {
        return PIPELINE_ERR_NULL_INPUT;
    }

    PipelineTaskContext tasks[GRID_POINT_COUNT];
    pthread_t readers[GRID_POINT_COUNT];
    pthread_t senders[GRID_POINT_COUNT];

    /* Inicjalizacja zadań */
    for (size_t i = 0; i < GRID_POINT_COUNT; i++) {
        const struct GridPoint *point = grid_get_point(i);

        tasks[i].pipeline_ctx = ctx;
        tasks[i].thread_id = (int) i;
        tasks[i].params.latitude = point->latitude;
        tasks[i].params.longitude = point->longitude;
        tasks[i].params.unit_type = CELCIUS;

        BufferResult br = shared_buffer_init(&tasks[i].buffer);
        if (br != BUFFER_OK) {
            /* Cleanup już zainicjalizowanych buforów */
            for (size_t j = 0; j < i; j++) {
                shared_buffer_destroy(&tasks[j].buffer);
            }
            return PIPELINE_ERR_INIT_FAILED;
        }
    }

    /* Start wątków — każda para reader+sender dla jednego punktu */
    for (size_t i = 0; i < GRID_POINT_COUNT; i++) {
        pthread_create(&senders[i], NULL, sender_thread, &tasks[i]);
        pthread_create(&readers[i], NULL, reader_thread, &tasks[i]);
    }

    /* Oczekiwanie na zakończenie */
    for (size_t i = 0; i < GRID_POINT_COUNT; i++) {
        pthread_join(readers[i], NULL);
        pthread_join(senders[i], NULL);
    }

    /* Cleanup buforów */
    for (size_t i = 0; i < GRID_POINT_COUNT; i++) {
        shared_buffer_destroy(&tasks[i].buffer);
    }

    return PIPELINE_OK;
}

void pipeline_cleanup(PipelineContext *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mqtt_publisher_disconnect(ctx->mqtt_ctx, ctx->mqtt_handle);
    mqtt_publisher_lib_cleanup();
    ctx->mqtt_handle = NULL;
}
