#include "mock_mqtt.h"
#include <string.h>

#define MOCK_BUFFER_SIZE 512U

static MqttResult  s_connect_result;
static MqttResult  s_publish_result;
static char        s_last_topic[MOCK_BUFFER_SIZE];
static char        s_last_payload[MOCK_BUFFER_SIZE];
static int         s_connect_count;
static int         s_publish_count;
static int         s_disconnect_count;

/* Dummy handle — cokolwiek niezerowe */
static int s_dummy_handle = 1;

void mock_mqtt_set_connect_result(MqttResult result)
{
    s_connect_result = result;
}

void mock_mqtt_set_publish_result(MqttResult result)
{
    s_publish_result = result;
}

const char *mock_mqtt_get_last_topic(void)
{
    return s_last_topic;
}

const char *mock_mqtt_get_last_payload(void)
{
    return s_last_payload;
}

int mock_mqtt_get_connect_call_count(void)  { return s_connect_count; }
int mock_mqtt_get_publish_call_count(void)  { return s_publish_count; }
int mock_mqtt_get_disconnect_call_count(void) { return s_disconnect_count; }

void mock_mqtt_reset(void)
{
    s_connect_result    = MQTT_OK;
    s_publish_result    = MQTT_OK;
    s_connect_count     = 0;
    s_publish_count     = 0;
    s_disconnect_count  = 0;
    memset(s_last_topic,   0, sizeof(s_last_topic));
    memset(s_last_payload, 0, sizeof(s_last_payload));
}

MqttResult mock_mqtt_connect(
    const char  *host,
    uint16_t     port,
    void       **client_handle)
{
    (void)host;
    (void)port;
    s_connect_count++;

    if (s_connect_result == MQTT_OK) {
        *client_handle = &s_dummy_handle;
    }

    return s_connect_result;
}

MqttResult mock_mqtt_publish(
    void       *client_handle,
    const char *topic,
    const char *payload,
    int         qos)
{
    (void)client_handle;
    (void)qos;
    s_publish_count++;

    strncpy(s_last_topic,   topic,   sizeof(s_last_topic)   - 1);
    strncpy(s_last_payload, payload, sizeof(s_last_payload) - 1);

    return s_publish_result;
}

void mock_mqtt_disconnect(void *client_handle)
{
    (void)client_handle;
    s_disconnect_count++;
}

const MqttPublisherOps MOCK_MQTT_OPS = {
    .connect    = mock_mqtt_connect,
    .publish    = mock_mqtt_publish,
    .disconnect = mock_mqtt_disconnect,
};