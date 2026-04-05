#include "mock_mqtt.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <bits/pthreadtypes.h>

#define MOCK_BUFFER_SIZE 512U

static MqttResult s_connect_result;
static MqttResult s_publish_result;
static char s_last_topic[MOCK_BUFFER_SIZE];
static char s_last_payload[MOCK_BUFFER_SIZE];
static char s_last_username[CONFIG_USERNAME_MAX_LEN];
static char s_last_password[CONFIG_PASSWORD_MAX_LEN];
static int s_connect_count;
static int s_publish_count;
static int s_disconnect_count;

/* Dummy handle — cokolwiek niezerowe */
static int s_dummy_handle = 1;
static pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;

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

int mock_mqtt_get_connect_call_count(void)
{
    pthread_mutex_lock(&s_mutex);
    int count = s_connect_count;
    pthread_mutex_unlock(&s_mutex);
    return count;
}

int mock_mqtt_get_publish_call_count(void)
{
    pthread_mutex_lock(&s_mutex);
    int count = s_publish_count;
    pthread_mutex_unlock(&s_mutex);
    return count;
}

int mock_mqtt_get_disconnect_call_count(void)
{
    pthread_mutex_lock(&s_mutex);
    int count = s_disconnect_count;
    pthread_mutex_unlock(&s_mutex);
    return count;
}

void mock_mqtt_reset(void)
{
    pthread_mutex_lock(&s_mutex);
    s_connect_result = MQTT_OK;
    s_publish_result = MQTT_OK;
    s_connect_count = 0;
    s_publish_count = 0;
    s_disconnect_count = 0;
    memset(s_last_topic, 0, sizeof(s_last_topic));
    memset(s_last_payload, 0, sizeof(s_last_payload));
    memset(s_last_username, 0, sizeof(s_last_username)); /* ← dodaj */
    memset(s_last_password, 0, sizeof(s_last_password)); /* ← dodaj */
    pthread_mutex_unlock(&s_mutex);
}

MqttResult mock_mqtt_connect(
    const char *host,
    uint16_t port,
    const char *username,
    const char *password,
    void **client_handle)
{
    pthread_mutex_lock(&s_mutex);
    (void) host;
    (void) port;
    s_connect_count++;

    if (username != NULL) {
        snprintf(s_last_username, sizeof(s_last_username),
                 "%s", username);
    }
    if (password != NULL) {
        snprintf(s_last_password, sizeof(s_last_password),
                 "%s", password);
    }

    if (s_connect_result == MQTT_OK) {
        *client_handle = &s_dummy_handle;
    }
    pthread_mutex_unlock(&s_mutex);

    return s_connect_result;
}

/* Getter do weryfikacji w testach */
const char *mock_mqtt_get_last_username(void)
{
    return s_last_username;
}

MqttResult mock_mqtt_publish(
    void *client_handle,
    const char *topic,
    const char *payload,
    int qos)
{
    pthread_mutex_lock(&s_mutex);
    s_publish_count++;
    strncpy(s_last_topic, topic, sizeof(s_last_topic) - 1);
    strncpy(s_last_payload, payload, sizeof(s_last_payload) - 1);
    pthread_mutex_unlock(&s_mutex);

    return s_publish_result;
}

void mock_mqtt_disconnect(void *client_handle)
{
    (void) client_handle;
    s_disconnect_count++;
}

const MqttPublisherOps MOCK_MQTT_OPS = {
    .connect = mock_mqtt_connect,
    .publish = mock_mqtt_publish,
    .disconnect = mock_mqtt_disconnect,
};
