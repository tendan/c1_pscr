#ifndef C1_MOCK_MQTT_H
#define C1_MOCK_MQTT_H

#include "mqtt/mqtt_publisher.h"

void mock_mqtt_set_connect_result(MqttResult result);
void mock_mqtt_set_publish_result(MqttResult result);

const char *mock_mqtt_get_last_topic(void);
const char *mock_mqtt_get_last_payload(void);
int         mock_mqtt_get_connect_call_count(void);
int         mock_mqtt_get_publish_call_count(void);
int         mock_mqtt_get_disconnect_call_count(void);

void mock_mqtt_reset(void);

MqttResult mock_mqtt_connect(
    const char  *host,
    uint16_t     port,
    void       **client_handle
);

MqttResult mock_mqtt_publish(
    void       *client_handle,
    const char *topic,
    const char *payload,
    int         qos
);

void mock_mqtt_disconnect(void *client_handle);

extern const MqttPublisherOps MOCK_MQTT_OPS;

#endif