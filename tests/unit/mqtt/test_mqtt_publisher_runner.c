
#include "unity_fixture.h"

TEST_GROUP_RUNNER(MqttPublisher)
{
    RUN_TEST_CASE(MqttPublisher, ContextFromConfigNullConfigReturnsNullInput);
    RUN_TEST_CASE(MqttPublisher, ContextFromConfigNullOpsReturnsNullInput);
    RUN_TEST_CASE(MqttPublisher, ContextFromConfigSetsHostCorrectly);
    RUN_TEST_CASE(MqttPublisher, ContextFromConfigSetsPortCorrectly);
    RUN_TEST_CASE(MqttPublisher, ContextFromConfigSetsTopicPrefixCorrectly);
    RUN_TEST_CASE(MqttPublisher, ConnectNullContextReturnsNullInput);
    RUN_TEST_CASE(MqttPublisher, ConnectHappyPathReturnsOk);
    RUN_TEST_CASE(MqttPublisher, ConnectSetsClientHandle);
    RUN_TEST_CASE(MqttPublisher, ConnectFailureReturnsConnectionFailed);
    RUN_TEST_CASE(MqttPublisher, ConnectCalledExactlyOnce);
    RUN_TEST_CASE(MqttPublisher, PublishNullContextReturnsNullInput);
    RUN_TEST_CASE(MqttPublisher, PublishNullDataReturnsNullInput);
    RUN_TEST_CASE(MqttPublisher, PublishHappyPathReturnsOk);
    RUN_TEST_CASE(MqttPublisher, PublishTopicContainsPrefix);
    RUN_TEST_CASE(MqttPublisher, PublishTopicContainsLatitude);
    RUN_TEST_CASE(MqttPublisher, PublishTopicContainsLongitude);
    RUN_TEST_CASE(MqttPublisher, PublishPayloadIsValidJson);
    RUN_TEST_CASE(MqttPublisher, PublishCalledExactlyOnce);
    RUN_TEST_CASE(MqttPublisher, DisconnectCalledExactlyOnce);
    RUN_TEST_CASE(MqttPublisher, ConnectWithCredentialsPropagatesUsername);
    RUN_TEST_CASE(MqttPublisher, ConnectWithoutCredentialsPassesNullUsername);
}