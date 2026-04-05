
#include "unity_fixture.h"
#include "config/config.h"

#include <stdio.h>

#define TEST_CONF_PATH "/tmp/test_pipeline.conf"

static void write_conf(const char *contents)
{
    FILE *f = fopen(TEST_CONF_PATH, "w");
    fputs(contents, f);
    fclose(f);
}

TEST_GROUP(Config);

TEST_SETUP(Config)    {}

TEST_TEAR_DOWN(Config)
{
    remove(TEST_CONF_PATH);
}

TEST(Config, NullPathReturnsInvalidValue)
{
    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_ERR_INVALID_VALUE,
        config_load(NULL, &cfg));
}

TEST(Config, NullOutputReturnsInvalidValue)
{
    TEST_ASSERT_EQUAL(CONFIG_ERR_INVALID_VALUE,
        config_load(TEST_CONF_PATH, NULL));
}

TEST(Config, MissingFileReturnsFileNotFound)
{
    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_ERR_FILE_NOT_FOUND,
        config_load("/tmp/nonexistent_pipeline.conf", &cfg));
}

TEST(Config, LoadsHostCorrectly)
{
    write_conf(
        "mqtt_host=localhost\n"
        "mqtt_port=1883\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
        "appid=test\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_OK, config_load(TEST_CONF_PATH, &cfg));
    TEST_ASSERT_EQUAL_STRING("localhost", cfg.mqtt_host);
}

TEST(Config, LoadsPortCorrectly)
{
    write_conf(
        "mqtt_host=localhost\n"
        "mqtt_port=1883\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
        "appid=test\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_OK, config_load(TEST_CONF_PATH, &cfg));
    TEST_ASSERT_EQUAL_UINT16(1883, cfg.mqtt_port);
}

TEST(Config, LoadsTopicPrefixCorrectly)
{
    write_conf(
        "mqtt_host=localhost\n"
        "mqtt_port=1883\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
        "appid=test\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_OK, config_load(TEST_CONF_PATH, &cfg));
    TEST_ASSERT_EQUAL_STRING("agh/kse/c1/weather", cfg.mqtt_topic_prefix);
}

TEST(Config, CommentsAndEmptyLinesAreIgnored)
{
    write_conf(
        "# To jest komentarz\n"
        "\n"
        "mqtt_host=localhost\n"
        "mqtt_port=1883\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
        "appid=test\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_OK, config_load(TEST_CONF_PATH, &cfg));
    TEST_ASSERT_EQUAL_STRING("localhost", cfg.mqtt_host);
}

TEST(Config, InvalidPortReturnsInvalidValue)
{
    write_conf(
        "mqtt_host=localhost\n"
        "mqtt_port=99999\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
        "appid=test\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_ERR_INVALID_VALUE,
        config_load(TEST_CONF_PATH, &cfg));
}

TEST(Config, MissingHostReturnsMissingKey)
{
    write_conf(
        "mqtt_port=1883\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
        "appid=test\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_ERR_MISSING_KEY,
        config_load(TEST_CONF_PATH, &cfg));
}

TEST(Config, LoadsAppidCorrectly)
{
    write_conf(
        "mqtt_host=localhost\n"
        "mqtt_port=1883\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
        "appid=test_token_123\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_OK, config_load(TEST_CONF_PATH, &cfg));
    TEST_ASSERT_EQUAL_STRING("test_token_123", cfg.appid);
}

TEST(Config, MissingAppidReturnsMissingKey)
{
    write_conf(
        "mqtt_host=localhost\n"
        "mqtt_port=1883\n"
        "mqtt_topic_prefix=agh/kse/c1/weather\n"
    );

    struct AppConfig cfg = {0};
    TEST_ASSERT_EQUAL(CONFIG_ERR_MISSING_KEY,
        config_load(TEST_CONF_PATH, &cfg));
}