#include "config/config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINE_BUFFER_SIZE 512U

static ConfigResult parse_line(const char *line, struct AppConfig *out)
{
    char key[64]  = {0};
    char value[CONFIG_HOST_MAX_LEN] = {0};

    /* Pomiń komentarze i puste linie */
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\0') {
        return CONFIG_OK;
    }

    if (sscanf(line, "%63[^=]=%255s", key, value) != 2) {
        return CONFIG_ERR_INVALID_VALUE;
    }

    if (strcmp(key, "mqtt_host") == 0) {
        strncpy(out->mqtt_host, value, CONFIG_HOST_MAX_LEN - 1);
    } else if (strcmp(key, "mqtt_port") == 0) {
        int port = atoi(value);
        if (port <= 0 || port > 65535) {
            return CONFIG_ERR_INVALID_VALUE;
        }
        out->mqtt_port = (uint16_t)port;
    } else if (strcmp(key, "mqtt_topic_prefix") == 0) {
        strncpy(out->mqtt_topic_prefix, value, CONFIG_TOPIC_MAX_LEN - 1);
    } else {
        return CONFIG_ERR_MISSING_KEY;
    }

    return CONFIG_OK;
}

ConfigResult config_load(const char *path, struct AppConfig *out)
{
    if (path == NULL || out == NULL) {
        return CONFIG_ERR_INVALID_VALUE;
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return CONFIG_ERR_FILE_NOT_FOUND;
    }

    memset(out, 0, sizeof(struct AppConfig));

    char line[LINE_BUFFER_SIZE];
    ConfigResult result = CONFIG_OK;

    while (fgets(line, sizeof(line), f) != NULL) {
        /* Usuń newline */
        line[strcspn(line, "\n")] = '\0';

        result = parse_line(line, out);
        if (result != CONFIG_OK) {
            fclose(f);
            return result;
        }
    }

    fclose(f);

    /* Walidacja że wymagane pola zostały ustawione */
    if (out->mqtt_host[0] == '\0') {
        return CONFIG_ERR_MISSING_KEY;
    }
    if (out->mqtt_port == 0) {
        return CONFIG_ERR_MISSING_KEY;
    }
    if (out->mqtt_topic_prefix[0] == '\0') {
        return CONFIG_ERR_MISSING_KEY;
    }

    return CONFIG_OK;
}