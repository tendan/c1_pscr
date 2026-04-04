cmake_minimum_required(VERSION 3.16)
project(c1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Flagi kompilacji
add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wno-unused-parameter
)

# cJSON
# Exclude cJSON tests from compiling, especially Unity framework inside
set(ENABLE_CJSON_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/cjson)
target_compile_options(cjson PRIVATE -w)

# Unity
add_subdirectory(lib/unity)
target_compile_options(unity PRIVATE -w)

target_compile_definitions(unity PUBLIC
        UNITY_INCLUDE_DOUBLE
)

add_library(unity_fixture STATIC
        lib/unity/extras/fixture/src/unity_fixture.c
        lib/unity/extras/memory/src/unity_memory.c
)

target_include_directories(unity_fixture PUBLIC
        lib/unity/extras/fixture/src
        lib/unity/extras/memory/src
)

target_link_libraries(unity_fixture PUBLIC unity)

target_compile_definitions(unity_fixture PUBLIC
        UNITY_INCLUDE_DOUBLE
)

target_compile_options(unity_fixture PRIVATE -w)

# libmosquitto
find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

# curl
find_package(CURL REQUIRED)

# Source files
add_library(c1_lib STATIC
    src/weather/weather_client.c
    src/weather/weather_parser.c
    src/weather/weather_transformer.c
    src/grid/grid.c
    src/config/config.c
    src/config/config.h
    src/weather/weather_serializer.c
    #src/json_parser.c
    #src/transformer.c
    #src/mqtt_publisher.c
    #src/c1.c
)
target_include_directories(c1_lib PUBLIC
    src/
)
target_link_libraries(c1_lib
    cjson
    ${MOSQUITTO_LIBRARIES}
    pthread
    CURL::libcurl
)
target_include_directories(c1_lib PRIVATE
    ${MOSQUITTO_INCLUDE_DIRS}
)

# Executable
add_executable(c1_app src/main.c)
target_link_libraries(c1_app c1_lib)

# Tests
enable_testing()
add_subdirectory(tests)