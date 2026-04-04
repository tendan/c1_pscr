function(add_integration_test test_name source_file)
    add_executable(${test_name} ${source_file})

    target_link_libraries(${test_name}
            c1_lib
            unity_fixture
            # Bez test_mocks — tu chcemy prawdziwe zależności
    )

    add_test(
            NAME ${test_name}
            COMMAND ${test_name}
    )

    set_tests_properties(${test_name} PROPERTIES
            LABELS "integration"
            # Dłuższy timeout — sieć, broker, I/O
            TIMEOUT 30
    )
endfunction()

#add_integration_test(test_http_to_transform  test_http_to_transform.c)
#add_integration_test(test_transform_to_mqtt  test_transform_to_mqtt.c)