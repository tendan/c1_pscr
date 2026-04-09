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
        buffer/test_shared_buffer.c
        buffer/test_shared_buffer_runner.c
        pipeline/test_pipeline.c
        pipeline/test_pipeline_runner.c
        all_unit_tests.c
)

#target_include_directories(unit_tests PRIVATE
#    ${CMAKE_SOURCE_DIR}/lib
#)

target_link_libraries(unit_tests
        unity_fixture
        c1_lib
        test_mocks
)

if(VALGRIND_EXECUTABLE)
    add_custom_target(valgrind
            COMMAND ${VALGRIND_EXECUTABLE}
            --error-exitcode=1
            --leak-check=full
            --track-origins=yes
            --show-leak-kinds=all
            --errors-for-leak-kinds=all
            --suppressions=${CMAKE_SOURCE_DIR}/tools/valgrind.supp
            --log-file=${CMAKE_BINARY_DIR}/valgrind-report.txt
            $<TARGET_FILE:unit_tests> -v
            DEPENDS unit_tests
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running Valgrind on unit tests..."
            VERBATIM
    )

    add_custom_target(valgrind-report
            COMMAND cat ${CMAKE_BINARY_DIR}/valgrind-report.txt
            DEPENDS valgrind
            COMMENT "Valgrind report:"
    )
endif()

add_test(NAME unit_tests COMMAND unit_tests)

set_tests_properties(unit_tests PROPERTIES
        LABELS "unit"
)