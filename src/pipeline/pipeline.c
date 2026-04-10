#include "pipeline/pipeline.h"
#include "weather/weather_transformer.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "logger/logger.h"


static void *reader_thread(void *arg)
{
    PipelineTaskContext *ctx = (PipelineTaskContext *) arg;

    for (size_t i = 0; i < ctx->point_count; i++) {
        const struct GridPoint *point = grid_get_point_from_array(
            ctx->pipeline_ctx->grid_point_array,
            ctx->point_offset + i
        );

        /* Ustaw parametry dla tego punktu */
        struct WeatherQueryParams params = {
            .latitude = point->latitude,
            .longitude = point->longitude,
            .unit_type = CELCIUS
        };

        struct RawWeatherData raw = {0};
        struct CalculatedWeatherData calc = {0};
        enum WeatherDataResult result;

        /* Retry loop */
        for (int attempt = 0; attempt < PIPELINE_MAX_RETRIES; attempt++) {
            result = receive_coordinates_weather_data(
                ctx->pipeline_ctx->weather_ctx,
                &params,
                &raw);

            if (result == READ_OK) break;
            if (result == READ_FORBIDDEN) {
                log_message(LEVEL_ERROR, "thread %d: token forbidden, aborting",
                          ctx->thread_id);
                shared_buffer_produce(&ctx->buffer,
                                      &calc, 0, READ_FORBIDDEN);
                return NULL;
            }

            log_message(LEVEL_WARN, "thread %d: timeout, retry %d/%d",
                     ctx->thread_id, attempt + 1, PIPELINE_MAX_RETRIES);
            sleep(1);
        }

        if (result != READ_OK) {
            log_message(LEVEL_WARN, "thread %d: all retries exhausted for point %zu",
                     ctx->thread_id, ctx->point_offset + i);
            shared_buffer_produce(&ctx->buffer, &calc, 0, result);
            continue; /* ← przejdź do następnego punktu */
        }

        enum TransformResult tr = estimate_irradiance(&raw, &calc);
        if (tr != TRANSFORM_OK) {
            shared_buffer_produce(&ctx->buffer, &calc, 0, READ_OTHER_ERROR);
            continue;
        }

        shared_buffer_produce(&ctx->buffer, &calc, 1, READ_OK);
    }

    return NULL;
}

static void *sender_thread(void *arg)
{
    PipelineTaskContext *ctx = (PipelineTaskContext *) arg;
    struct CalculatedWeatherData data = {0};
    int valid = 0;
    enum WeatherDataResult status = READ_OK;

    for (size_t i = 0; i < ctx->point_count; i++) {
        BufferResult br = shared_buffer_consume(
            &ctx->buffer, &data, &valid, &status);

        if (br != BUFFER_OK) {
            log_message(LEVEL_WARN, "thread %d: buffer consume failed: %d",
                     ctx->thread_id, br);
            continue;
        }

        if (!valid) {
            log_message(LEVEL_WARN, "thread %d: data invalid (status=%d), skipping publish",
                     ctx->thread_id, status);
            continue;
        }

        MqttResult mr = mqtt_publisher_publish(
            ctx->pipeline_ctx->mqtt_ctx,
            ctx->pipeline_ctx->mqtt_handle,
            &data);

        if (mr != MQTT_OK) {
            log_message(LEVEL_WARN, "thread %d: publish failed: %d",
                     ctx->thread_id, mr);
        }
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

    if (ctx->grid_point_array->count == 0) {
        log_message(LEVEL_ERROR, "pipeline: no grid points to process");
        return PIPELINE_ERR_INIT_FAILED;
    }

    PipelineTaskContext tasks[PIPELINE_THREAD_COUNT];
    pthread_t readers[PIPELINE_THREAD_COUNT];
    pthread_t senders[PIPELINE_THREAD_COUNT];

    size_t base = ctx->grid_point_array->count / PIPELINE_THREAD_COUNT;
    size_t reste = ctx->grid_point_array->count % PIPELINE_THREAD_COUNT;

    log_message(LEVEL_INFO, "pipeline: %zu points, %d threads, base=%zu, reste=%zu",
             ctx->grid_point_array->count, PIPELINE_THREAD_COUNT, base, reste);


    for (size_t i = 0; i < PIPELINE_THREAD_COUNT; i++) {
        size_t extra = (i < reste) ? 1 : 0;
        size_t offset = i * base + (i < reste ? i : reste);

        tasks[i].pipeline_ctx = ctx;
        tasks[i].thread_id = (int) i;
        tasks[i].point_offset = offset;
        tasks[i].point_count = base + extra;

        BufferResult br = shared_buffer_init(&tasks[i].buffer);
        if (br != BUFFER_OK) {
            for (size_t j = 0; j < i; j++) {
                shared_buffer_destroy(&tasks[j].buffer);
            }
            return PIPELINE_ERR_INIT_FAILED;
        }

        log_message(LEVEL_DEBUG, "pipeline: thread %zu → points [%zu, %zu)",
                  i, offset, offset + base + extra);
    }

    /* Start wątków */
    for (size_t i = 0; i < PIPELINE_THREAD_COUNT; i++) {
        pthread_create(&senders[i], NULL, sender_thread, &tasks[i]);
        pthread_create(&readers[i], NULL, reader_thread, &tasks[i]);
    }

    /* Join */
    for (size_t i = 0; i < PIPELINE_THREAD_COUNT; i++) {
        pthread_join(readers[i], NULL);
        pthread_join(senders[i], NULL);
    }

    /* Cleanup */
    for (size_t i = 0; i < PIPELINE_THREAD_COUNT; i++) {
        shared_buffer_destroy(&tasks[i].buffer);
    }

    log_message(LEVEL_INFO, "pipeline: completed successfully");
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
