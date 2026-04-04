#include "mqtt/mqtt_publisher.h"
#include "weather/weather_serializer.h"

#include <stdio.h>
#include <mosquitto.h>
#include <string.h>

#define TOPIC_BUFFER_SIZE  256U
#define PAYLOAD_BUFFER_SIZE WEATHER_SERIALIZER_BUFFER_SIZE

MqttResult mqtt_publisher_context_from_config(
    const struct AppConfig      *config,
    const MqttPublisherOps      *ops,
    struct MqttPublisherContext *out)
{
    if (config == NULL || ops == NULL || out == NULL) {
        return MQTT_ERR_NULL_INPUT;
    }

    out->host         = config->mqtt_host;
    out->port         = config->mqtt_port;
    out->topic_prefix = config->mqtt_topic_prefix;
    out->ops          = *ops;

    return MQTT_OK;
}

MqttResult mqtt_publisher_connect(
    struct MqttPublisherContext *ctx,
    void                       **client_handle)
{
    if (ctx == NULL || client_handle == NULL) {
        return MQTT_ERR_NULL_INPUT;
    }

    return ctx->ops.connect(ctx->host, ctx->port, client_handle);
}

MqttResult mqtt_publisher_publish(
    struct MqttPublisherContext        *ctx,
    void                               *client_handle,
    const struct CalculatedWeatherData *data)
{
    if (ctx == NULL || client_handle == NULL || data == NULL) {
        return MQTT_ERR_NULL_INPUT;
    }

    /* Budowanie topicu */
    char topic[TOPIC_BUFFER_SIZE];
    int written = snprintf(topic, sizeof(topic),
        "%s/%.2f/%.2f",
        ctx->topic_prefix,
        (double)data->latitude,
        (double)data->longitude);

    if (written < 0 || (size_t)written >= sizeof(topic)) {
        return MQTT_ERR_PUBLISH_FAILED;
    }

    /* Serializacja payloadu */
    char payload[PAYLOAD_BUFFER_SIZE];
    SerializeResult ser_result =
        serialize_weather_data(data, payload, sizeof(payload));

    if (ser_result != SERIALIZE_OK) {
        return MQTT_ERR_PUBLISH_FAILED;
    }

    return ctx->ops.publish(client_handle, topic, payload, MQTT_QOS);
}

void mqtt_publisher_disconnect(
    struct MqttPublisherContext *ctx,
    void                        *client_handle)
{
    if (ctx == NULL || client_handle == NULL) {
        return;
    }

    ctx->ops.disconnect(client_handle);
}

MqttResult mqtt_publisher_lib_init(void)
{
    if (mosquitto_lib_init() != MOSQ_ERR_SUCCESS) {
        return MQTT_ERR_CONNECTION_FAILED;
    }
    return MQTT_OK;
}

void mqtt_publisher_lib_cleanup(void)
{
    mosquitto_lib_cleanup();
}

/* ── Implementacje ops ─────────────────────────────────────── */

static MqttResult mosquitto_connect_impl(
    const char  *host,
    uint16_t     port,
    void       **client_handle)
{
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (mosq == NULL) {
        return MQTT_ERR_CONNECTION_FAILED;
    }

    int rc = mosquitto_connect(mosq, host, (int)port, 60);

    if (rc == MOSQ_ERR_AUTH) {
        mosquitto_destroy(mosq);
        return MQTT_ERR_AUTH_FAILED;
    }

    if (rc != MOSQ_ERR_SUCCESS) {
        mosquitto_destroy(mosq);
        return MQTT_ERR_CONNECTION_FAILED;
    }

    *client_handle = mosq;
    return MQTT_OK;
}

static MqttResult mosquitto_publish_impl(
    void       *client_handle,
    const char *topic,
    const char *payload,
    int         qos)
{
    struct mosquitto *mosq = (struct mosquitto *)client_handle;

    int rc = mosquitto_publish(
        mosq,
        NULL,                   /* message id — nie potrzebujemy */
        topic,
        (int)strlen(payload),
        payload,
        qos,
        false                   /* retain */
    );

    if (rc != MOSQ_ERR_SUCCESS) {
        return MQTT_ERR_PUBLISH_FAILED;
    }

    return MQTT_OK;
}

static void mosquitto_disconnect_impl(void *client_handle)
{
    struct mosquitto *mosq = (struct mosquitto *)client_handle;
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
}

const MqttPublisherOps MOSQUITTO_OPS = {
    .connect    = mosquitto_connect_impl,
    .publish    = mosquitto_publish_impl,
    .disconnect = mosquitto_disconnect_impl,
};