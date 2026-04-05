add_library(test_mocks STATIC
        mock_http.c
        mock_mqtt.c
)

target_include_directories(test_mocks PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_mocks PUBLIC
        c1_lib
)