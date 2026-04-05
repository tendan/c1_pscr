//
// Created by tendan on 4.04.2026.
//

#ifndef C1_CONFIG_H
#define C1_CONFIG_H

#define CONFIG_HOST_MAX_LEN   256U
#define CONFIG_TOPIC_MAX_LEN  256U
#define CONFIG_APPID_MAX_LEN  64U
#define CONFIG_PATH_DEFAULT   "/etc/pipeline/pipeline.conf"
#include <stdint.h>

struct AppConfig {
    char     mqtt_host[CONFIG_HOST_MAX_LEN];
    uint16_t mqtt_port;
    char     mqtt_topic_prefix[CONFIG_TOPIC_MAX_LEN];
    char     appid[CONFIG_APPID_MAX_LEN];
};

typedef enum {
    CONFIG_OK = 0,
    CONFIG_ERR_FILE_NOT_FOUND,
    CONFIG_ERR_MISSING_KEY,
    CONFIG_ERR_INVALID_VALUE,
} ConfigResult;

ConfigResult config_load(const char *path, struct AppConfig *out);

#endif //C1_CONFIG_H