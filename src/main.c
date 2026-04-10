#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "main_helpers.h"
#include "grid/grid_loader.h"
#include "logger/logger.h"
#include "config/config.h"
#include "weather/weather_client.h"
#include "pipeline/pipeline.h"

#define CONFIG_PATH "/etc/c1/c1_app.conf"

static const struct option long_options[] = {
    {"grid", required_argument, NULL, 'g'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
};

static void print_usage(const char *prog)
{
    fprintf(stderr,
            "Usage: %s [-g|--grid <path>]\n"
            "  -g, --grid <path>  Path to grid CSV file\n"
            "                     (default: %s)\n",
            prog, DEFAULT_GRID_PATH);
}

int main(int argc, char *argv[])
{
    const char *cli_grid_path = NULL;
    int opt;

    while ((opt = getopt_long(argc, argv, "g:h",
                              long_options, NULL)) != -1) {
        switch (opt) {
            case 'g':
                cli_grid_path = optarg;
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    logger_init("c1_app");

    struct AppConfig config = {0};
    ConfigResult config_result = config_load(CONFIG_PATH, &config);
    if (config_result != CONFIG_OK) {
        log_message(LEVEL_ERROR, "Failed to load config from %s: %d",
                    CONFIG_PATH, config_result);
        logger_cleanup();
        return EXIT_FAILURE;
    }

    GridPointArray grid = {0};
    const char *path = resolve_grid_path(
        cli_grid_path,
        DEFAULT_GRID_PATH
    );

    if (path != NULL) {
        GridLoadResult gr = grid_load_from_file(path, &grid);
        if (gr != GRID_LOAD_OK) {
            log_message(LEVEL_WARN, "Failed to load grid from %s — using fallback",
                        path);
        }
    }

    if (grid.count == 0) {
        grid_load_fallback(&grid);
    }

    log_message(LEVEL_INFO, "Starting pipeline with %zu grid points", grid.count);

    struct WeatherClientContext weather_ctx = {
        .endpoint = "https://api.openweathermap.org/data/2.5/weather",
        .appid = config.appid,
        .client_ops = {.fetch = curl_fetch}
    };

    struct MqttPublisherContext mqtt_ctx = {0};
    MqttResult mqtt_result = mqtt_publisher_context_from_config(
        &config, &MOSQUITTO_OPS, &mqtt_ctx);

    if (mqtt_result != MQTT_OK) {
        log_message(LEVEL_ERROR, "Failed to create MQTT context: %d", mqtt_result);
        logger_cleanup();
        return EXIT_FAILURE;
    }

    PipelineContext pipeline_ctx = {
        .weather_ctx = &weather_ctx,
        .mqtt_ctx = &mqtt_ctx,
        .mqtt_handle = NULL,
        .grid_point_array = &grid,
    };

    PipelineResult result = pipeline_init(&pipeline_ctx);
    if (result != PIPELINE_OK) {
        log_message(LEVEL_ERROR, "Failed to initialize pipeline: %d", result);
        logger_cleanup();
        return EXIT_FAILURE;
    }

    result = pipeline_run(&pipeline_ctx);
    if (result != PIPELINE_OK) {
        log_message(LEVEL_ERROR, "Pipeline run failed: %d", result);
        pipeline_cleanup(&pipeline_ctx);
        logger_cleanup();
        return EXIT_FAILURE;
    }

    pipeline_cleanup(&pipeline_ctx);
    logger_cleanup();
    return EXIT_SUCCESS;
}
