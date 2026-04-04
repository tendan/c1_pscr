
#include <unity_fixture.h>

#include "weather/weather_parser.h"

TEST_GROUP(WeatherParser);

TEST_SETUP(WeatherParser)
{

}

TEST_TEAR_DOWN(WeatherParser)
{

}

TEST(WeatherParser, ParsesLatitudeCorrectly)
{
    const char *response_buf = "{\"coord\":{\"lon\":19.94,\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":0},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_OK, result);
    TEST_ASSERT_EQUAL_FLOAT(50.06, out.latitude);
}

TEST(WeatherParser, ParsesLongitudeCorrectly)
{
    const char *response_buf = "{\"coord\":{\"lon\":19.94,\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":0},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_OK, result);
    TEST_ASSERT_EQUAL_FLOAT(19.94, out.longitude);
}

TEST(WeatherParser, ParsesTemperatureCorrectly)
{
    const char *response_buf = "{\"coord\":{\"lon\":19.94,\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":0},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_OK, result);
    TEST_ASSERT_EQUAL_FLOAT(9.48, out.temperature);
}

TEST(WeatherParser, ParsesWindCorrectly)
{
    const char *response_buf = "{\"coord\":{\"lon\":19.94,\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":0},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_OK, result);
    TEST_ASSERT_EQUAL_FLOAT(2.57, out.wind_speed);
    TEST_ASSERT_EQUAL_FLOAT(310, out.wind_degrees);
}

TEST(WeatherParser, ParsesCloudinessCorrectly)
{
    const char *response_buf = "{\"coord\":{\"lon\":19.94,\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":45},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_OK, result);
    TEST_ASSERT_EQUAL_INT(45, out.cloudiness);
}

TEST(WeatherParser, MissingFieldReturnsError)
{
    const char *response_buf = "{\"coord\":{\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":0},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_MISSING_FIELD, result);
}

TEST(WeatherParser, MalformedJsonReturnsError)
{
    const char *response_buf = "{\"coord:{\"lon\":19.94\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":0},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_MALFORMED, result);
}

TEST(WeatherParser, NullBufferReturnsError)
{
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(NULL, &out);

    TEST_ASSERT_EQUAL(PARSE_BUFFER_EMPTY, result);
}

TEST(WeatherParser, CorrectJsonDoesNotReturnError)
{
    const char *response_buf = "{\"coord\":{\"lon\":19.94,\"lat\":50.06},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clearsky\",\"icon\":\"01n\"}],\"base\":\"stations\",\"main\":{\"temp\":9.48,\"feels_like\":8.16,\"temp_min\":9.16,\"temp_max\":9.71,\"pressure\":1017,\"humidity\":22,\"sea_level\":1017,\"grnd_level\":986},\"visibility\":10000,\"wind\":{\"speed\":2.57,\"deg\":310},\"clouds\":{\"all\":0},\"dt\":1775245710,\"sys\":{\"type\":2,\"id\":2095241,\"country\":\"PL\",\"sunrise\":1775189575,\"sunset\":1775236433},\"timezone\":7200,\"id\":3085041,\"name\":\"Śródmieście\",\"cod\":200}";
    struct RawWeatherData out = {0};
    enum ParseResult result = parse_weather_response(response_buf, &out);

    TEST_ASSERT_EQUAL(PARSE_OK, result);
}