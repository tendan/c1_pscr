#include "unity_fixture.h"
#include "mqtt/mqtt_publisher.h"
#include "mock_mqtt.h"

#include <string.h>

static struct MqttPublisherContext s_ctx;
static struct AppConfig            s_config;
static void                       *s_handle;

static const struct CalculatedWeatherData s_data = {
    .latitude     = 52.23f,
    .longitude    = 21.01f,
    .temperature  = 20.50f,
    .irradiance   = 650.30f,
    .wind_speed   = 3.20f,
    .wind_degrees = 270
};

TEST_GROUP(MqttPublisher);

TEST_SETUP(MqttPublisher)
{
    mock_mqtt_reset();
    s_handle = NULL;

    s_config = (struct AppConfig){
        .mqtt_port           = 1883,
    };
    strncpy(s_config.mqtt_host,         "localhost",        CONFIG_HOST_MAX_LEN  - 1);
    strncpy(s_config.mqtt_topic_prefix, "agh/kse/c1/weather", CONFIG_TOPIC_MAX_LEN - 1);

    mqtt_publisher_context_from_config(&s_config, &MOCK_MQTT_OPS, &s_ctx);
}

TEST_TEAR_DOWN(MqttPublisher)
{
    mock_mqtt_reset();
}



TEST(MqttPublisher, ContextFromConfigNullConfigReturnsNullInput)
{
    TEST_ASSERT_EQUAL(MQTT_ERR_NULL_INPUT,
        mqtt_publisher_context_from_config(NULL, &MOCK_MQTT_OPS, &s_ctx));
}

TEST(MqttPublisher, ContextFromConfigNullOpsReturnsNullInput)
{
    TEST_ASSERT_EQUAL(MQTT_ERR_NULL_INPUT,
        mqtt_publisher_context_from_config(&s_config, NULL, &s_ctx));
}

TEST(MqttPublisher, ContextFromConfigSetsHostCorrectly)
{
    TEST_ASSERT_EQUAL_STRING("localhost", s_ctx.host);
}

TEST(MqttPublisher, ContextFromConfigSetsPortCorrectly)
{
    TEST_ASSERT_EQUAL_UINT16(1883, s_ctx.port);
}

TEST(MqttPublisher, ContextFromConfigSetsTopicPrefixCorrectly)
{
    TEST_ASSERT_EQUAL_STRING("agh/kse/c1/weather", s_ctx.topic_prefix);
}



TEST(MqttPublisher, ConnectNullContextReturnsNullInput)
{
    TEST_ASSERT_EQUAL(MQTT_ERR_NULL_INPUT,
        mqtt_publisher_connect(NULL, &s_handle));
}

TEST(MqttPublisher, ConnectHappyPathReturnsOk)
{
    TEST_ASSERT_EQUAL(MQTT_OK,
        mqtt_publisher_connect(&s_ctx, &s_handle));
}

TEST(MqttPublisher, ConnectSetsClientHandle)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    TEST_ASSERT_NOT_NULL(s_handle);
}

TEST(MqttPublisher, ConnectFailureReturnsConnectionFailed)
{
    mock_mqtt_set_connect_result(MQTT_ERR_CONNECTION_FAILED);
    TEST_ASSERT_EQUAL(MQTT_ERR_CONNECTION_FAILED,
        mqtt_publisher_connect(&s_ctx, &s_handle));
}

TEST(MqttPublisher, ConnectCalledExactlyOnce)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    TEST_ASSERT_EQUAL_INT(1, mock_mqtt_get_connect_call_count());
}



TEST(MqttPublisher, PublishNullContextReturnsNullInput)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    TEST_ASSERT_EQUAL(MQTT_ERR_NULL_INPUT,
        mqtt_publisher_publish(NULL, s_handle, &s_data));
}

TEST(MqttPublisher, PublishNullDataReturnsNullInput)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    TEST_ASSERT_EQUAL(MQTT_ERR_NULL_INPUT,
        mqtt_publisher_publish(&s_ctx, s_handle, NULL));
}

TEST(MqttPublisher, PublishHappyPathReturnsOk)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    TEST_ASSERT_EQUAL(MQTT_OK,
        mqtt_publisher_publish(&s_ctx, s_handle, &s_data));
}

TEST(MqttPublisher, PublishTopicContainsPrefix)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    mqtt_publisher_publish(&s_ctx, s_handle, &s_data);

    TEST_ASSERT_NOT_NULL(
        strstr(mock_mqtt_get_last_topic(), "agh/kse/c1/weather"));
}

TEST(MqttPublisher, PublishTopicContainsLatitude)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    mqtt_publisher_publish(&s_ctx, s_handle, &s_data);

    TEST_ASSERT_NOT_NULL(
        strstr(mock_mqtt_get_last_topic(), "52.23"));
}

TEST(MqttPublisher, PublishTopicContainsLongitude)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    mqtt_publisher_publish(&s_ctx, s_handle, &s_data);

    TEST_ASSERT_NOT_NULL(
        strstr(mock_mqtt_get_last_topic(), "21.01"));
}

TEST(MqttPublisher, PublishPayloadIsValidJson)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    mqtt_publisher_publish(&s_ctx, s_handle, &s_data);

    const char *payload = mock_mqtt_get_last_payload();
    TEST_ASSERT_NOT_NULL(payload);
    TEST_ASSERT_GREATER_THAN(0, strlen(payload));
}

TEST(MqttPublisher, PublishCalledExactlyOnce)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    mqtt_publisher_publish(&s_ctx, s_handle, &s_data);
    TEST_ASSERT_EQUAL_INT(1, mock_mqtt_get_publish_call_count());
}



TEST(MqttPublisher, DisconnectCalledExactlyOnce)
{
    mqtt_publisher_connect(&s_ctx, &s_handle);
    mqtt_publisher_disconnect(&s_ctx, s_handle);
    TEST_ASSERT_EQUAL_INT(1, mock_mqtt_get_disconnect_call_count());
}

TEST(MqttPublisher, ConnectWithCredentialsPropagatesUsername)
{
    strncpy(s_config.mqtt_username, "testuser",
        CONFIG_USERNAME_MAX_LEN - 1);
    strncpy(s_config.mqtt_password, "testpass",
        CONFIG_PASSWORD_MAX_LEN - 1);

    mqtt_publisher_context_from_config(
        &s_config, &MOCK_MQTT_OPS, &s_ctx);
    mqtt_publisher_connect(&s_ctx, &s_handle);

    TEST_ASSERT_EQUAL_STRING("testuser",
        mock_mqtt_get_last_username());
}

TEST(MqttPublisher, ConnectWithoutCredentialsPassesNullUsername)
{
    /* s_config ma puste username z TEST_SETUP */
    TEST_ASSERT_EQUAL(MQTT_OK,
        mqtt_publisher_connect(&s_ctx, &s_handle));
    TEST_ASSERT_EQUAL_STRING("", mock_mqtt_get_last_username());
}