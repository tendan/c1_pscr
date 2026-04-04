add_executable(unit_tests
        grid/test_grid.c
        grid/test_grid_runner.c
        weather/test_weather_client.c
        weather/test_weather_client_runner.c
        weather/test_weather_parser.c
        weather/test_weather_parser_runner.c
        weather/test_weather_client_internal.c
        weather/test_weather_transformer.c
        weather/test_weather_transformer_runner.c
        weather/test_weather_serializer.c
        weather/test_weather_serializer_runner.c
        config/test_config.c
        config/test_config_runner.c
        mqtt/test_mqtt_publisher.c
        mqtt/test_mqtt_publisher_runner.c
        all_unit_tests.c
)

target_link_libraries(unit_tests
        unity_fixture
        c1_lib
        test_mocks
)

add_test(NAME unit_tests COMMAND unit_tests)