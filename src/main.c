#include <stdio.h>
#include <stdlib.h>

#include "config/config.h"
#include "weather/weather_client.h"
#include "mqtt/mqtt_publisher.h"
#include "pipeline/pipeline.h"

#define CONFIG_PATH "/etc/c1/c1_app.conf"

int main(void)
{
    struct AppConfig config = {0};

    ConfigResult config_result = config_load(CONFIG_PATH, &config);
    if (config_result != CONFIG_OK) {
        fprintf(stderr, "Failed to load config from %s: %d\n",
            CONFIG_PATH, config_result);
        return EXIT_FAILURE;
    }

    struct WeatherClientContext weather_ctx = {
        .endpoint   = "https://api.openweathermap.org/data/2.5/weather",
        .appid      = config.appid,
        .client_ops = { .fetch = curl_fetch }
    };

    struct MqttPublisherContext mqtt_ctx = {0};

    MqttResult mqtt_result = mqtt_publisher_context_from_config(
        &config, &MOSQUITTO_OPS, &mqtt_ctx);

    if (mqtt_result != MQTT_OK) {
        fprintf(stderr, "Failed to create MQTT context: %d\n",
            mqtt_result);
        return EXIT_FAILURE;
    }

    PipelineContext pipeline_ctx = {
        .weather_ctx = &weather_ctx,
        .mqtt_ctx    = &mqtt_ctx,
        .mqtt_handle = NULL,
    };

    PipelineResult result = pipeline_init(&pipeline_ctx);
    if (result != PIPELINE_OK) {
        fprintf(stderr, "Failed to initialize pipeline: %d\n", result);
        return EXIT_FAILURE;
    }

    result = pipeline_run(&pipeline_ctx);
    if (result != PIPELINE_OK) {
        fprintf(stderr, "Pipeline run failed: %d\n", result);
        pipeline_cleanup(&pipeline_ctx);
        return EXIT_FAILURE;
    }

    pipeline_cleanup(&pipeline_ctx);
    return EXIT_SUCCESS;
}