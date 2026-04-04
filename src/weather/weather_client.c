//
// Created by tendan on 31.03.2026.
//

#include "weather_client.h"

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "weather_client_internal.h"

#define RESPONSE_BUFFER_SIZE 4096U
#define URL_BUFFER_SIZE 512U

static const char *unit_type_to_str(enum UnitType unit_type)
{
    switch (unit_type) {
        case KELVIN: return "standard";
        case CELCIUS: return "metric";
        case FAHRENHEIT: return "fahrenheit";
        default: return "metric";
    }
}

static size_t weather_write_callback(
    void   *contents,
    size_t  size,
    size_t  nmemb,
    void   *userdata)
{
    struct CurlWriteContext *ctx    = (struct CurlWriteContext*)userdata;
    size_t            chunk  = size * nmemb;

    if (ctx->written + chunk >= ctx->buf_len - 1) {
        chunk = ctx->buf_len - ctx->written - 1;
    }

    memcpy(ctx->buf + ctx->written, contents, chunk);
    ctx->written += chunk;
    ctx->buf[ctx->written] = '\0';

    return size * nmemb;
}

static enum HttpResult http_code_to_result(long http_code)
{
    switch (http_code)
    {
        case 200: return HTTP_OK;
        case 400: return HTTP_BAD_REQUEST;
        case 401: return HTTP_FORBIDDEN;
        case 404: return HTTP_NOT_FOUND;
        case 500: return HTTP_INTERNAL_SERVER_ERROR;
        default:  return HTTP_OTHER_ERROR;
    }
}

enum HttpResult curl_fetch(const char *url, char *buf, size_t buf_len)
{
    CURL    *curl;
    CURLcode res;

    struct CurlWriteContext write_ctx = {
        .buf     = buf,
        .buf_len = buf_len,
        .written = 0
    };

    curl = curl_easy_init();
    if (!curl) {
        return HTTP_OTHER_ERROR;
    }

    curl_easy_setopt(curl, CURLOPT_URL,            url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  weather_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &write_ctx);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        10L);

    res = curl_easy_perform(curl);

    if (res == CURLE_OPERATION_TIMEDOUT) {
        curl_easy_cleanup(curl);
        return HTTP_TIMEOUT;
    }

    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return HTTP_OTHER_ERROR;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    return http_code_to_result(http_code);
}

enum HttpResult perform_http_request(
    const struct WeatherClientContext *ctx,
    const struct WeatherQueryParams *params,
    char *response_buf,
    size_t buf_len
)
{
    char url[URL_BUFFER_SIZE];

    int written = snprintf(url, sizeof(url),
        "%s?appid=%s&lat=%.7f&lon=%.7f&units=%s",
        ctx->endpoint,
        ctx->appid,
        (double)params->latitude,
        (double)params->longitude,
        unit_type_to_str(params->unit_type));

    if (written < 0 || (size_t)written >= sizeof(url)) {
        return HTTP_BAD_REQUEST;
    }

    memset(response_buf, 0, buf_len);
    return ctx->client_ops.fetch(url, response_buf, buf_len);
}

enum WeatherDataResult receive_coordinates_weather_data(const struct WeatherClientContext *ctx,
                                                        const struct WeatherQueryParams *params,
                                                        struct RawWeatherData *raw_weather_data)
{
    char response_buf[RESPONSE_BUFFER_SIZE];

    enum HttpResult http_result = perform_http_request(ctx, params, response_buf, sizeof(response_buf));

    switch (http_result) {
        case HTTP_OK:
            break;
        case HTTP_TIMEOUT:
            return READ_TIMEOUT;
        case HTTP_FORBIDDEN:
            return READ_FORBIDDEN;
        default:
            return READ_OTHER_ERROR;
    }

    enum ParseResult parse_result = parse_weather_response(response_buf, raw_weather_data);

    if (parse_result != PARSE_OK) {
        return READ_OTHER_ERROR;
    }

    return READ_OK;
}
