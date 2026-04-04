add_executable(unit_tests
        test_grid.c
        test_grid_runner.c
        test_weather_client.c
        test_weather_client_runner.c
        test_weather_parser.c
        test_weather_parser_runner.c
        test_weather_client_internal.c
        all_unit_tests.c
)

target_link_libraries(unit_tests
        unity_fixture
        c1_lib
        test_mocks
)

add_test(NAME unit_tests COMMAND unit_tests)