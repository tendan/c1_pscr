add_executable(integration_tests
        test_http_to_buffer.c
        test_http_to_buffer_runner.c
        test_buffer_to_mqtt.c
        test_buffer_to_mqtt_runner.c
        test_pipeline_full.c
        test_pipeline_full_runner.c
        all_integration_tests_main.c
)

target_link_libraries(integration_tests
        unity_fixture
        c1_lib
)

# Integration tests setup
add_test(
        NAME integration_setup
        COMMAND ${CMAKE_SOURCE_DIR}/tools/integration_setup.sh
        start
        ${CMAKE_SOURCE_DIR}
)

set_tests_properties(integration_setup PROPERTIES
        FIXTURES_SETUP    integration_infrastructure
        LABELS            "integration"
        TIMEOUT           15
)

# Integration tests cleanup
add_test(
        NAME integration_cleanup
        COMMAND ${CMAKE_SOURCE_DIR}/tools/integration_setup.sh
        stop
        ${CMAKE_SOURCE_DIR}
)

set_tests_properties(integration_cleanup PROPERTIES
        FIXTURES_CLEANUP  integration_infrastructure
        LABELS            "integration"
        TIMEOUT           10
)


add_test(
        NAME integration_tests
        COMMAND integration_tests -v
)

set_tests_properties(integration_tests PROPERTIES
        FIXTURES_REQUIRED integration_infrastructure
        LABELS            "integration"
        TIMEOUT           120
)