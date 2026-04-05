//
// Created by tendan on 4.04.2026.
//

#ifndef C1_MQTT_PUBLISHER_H
#define C1_MQTT_PUBLISHER_H

#include <stdint.h>
#include "config/config.h"
#include "weather/weather_transformer.h"

#define MQTT_QOS 1

typedef enum {
    MQTT_OK = 0,
    MQTT_ERR_NULL_INPUT,
    MQTT_ERR_CONNECTION_FAILED,
    MQTT_ERR_AUTH_FAILED,
    MQTT_ERR_PUBLISH_FAILED,
} MqttResult;

typedef MqttResult (*mqtt_connect_fn)(
    const char *host,
    uint16_t port,
    const char *username,
    const char *password,
    void **client_handle);

typedef MqttResult (*mqtt_publish_fn)(
    void *client_handle,
    const char *topic,
    const char *payload,
    int qos
);

typedef void (*mqtt_disconnect_fn)(void *client_handle);

typedef struct {
    mqtt_connect_fn connect;
    mqtt_publish_fn publish;
    mqtt_disconnect_fn disconnect;
} MqttPublisherOps;

struct MqttPublisherContext {
    const char *host;
    uint16_t port;
    const char *topic_prefix;
    const char *username;
    const char *password;
    MqttPublisherOps ops;
};

MqttResult mqtt_publisher_context_from_config(
    const struct AppConfig *config,
    const MqttPublisherOps *ops,
    struct MqttPublisherContext *out
);

MqttResult mqtt_publisher_connect(
    struct MqttPublisherContext *ctx,
    void **client_handle
);

MqttResult mqtt_publisher_publish(
    struct MqttPublisherContext *ctx,
    void *client_handle,
    const struct CalculatedWeatherData *data
);

void mqtt_publisher_disconnect(
    struct MqttPublisherContext *ctx,
    void *client_handle
);

MqttResult mqtt_publisher_lib_init(void);

void mqtt_publisher_lib_cleanup(void);

extern const MqttPublisherOps MOSQUITTO_OPS;

#endif
