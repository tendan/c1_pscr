#ifndef C1_PIPELINE_H
#define C1_PIPELINE_H

#include "weather/weather_client.h"
#include "mqtt/mqtt_publisher.h"
#include "buffer/shared_buffer.h"
#include "grid/grid.h"
#include "grid/grid_loader.h"

#define PIPELINE_MAX_RETRIES 3

typedef enum {
    PIPELINE_OK = 0,
    PIPELINE_ERR_NULL_INPUT,
    PIPELINE_ERR_INIT_FAILED,
    PIPELINE_ERR_FATAL,
} PipelineResult;

typedef struct {
    const struct WeatherClientContext *weather_ctx;
    struct MqttPublisherContext       *mqtt_ctx;
    void                              *mqtt_handle;
    GridPointArray                    *grid_point_array;
} PipelineContext;

typedef struct {
    const PipelineContext      *pipeline_ctx;
    struct WeatherQueryParams   params;
    SharedBuffer                buffer;
    int                         thread_id;
} PipelineTaskContext;

PipelineResult pipeline_init(PipelineContext *ctx);
PipelineResult pipeline_run(const PipelineContext *ctx);
void           pipeline_cleanup(PipelineContext *ctx);

#endif