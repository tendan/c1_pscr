//
// Created by tendan on 31.03.2026.
//

#ifndef C1_MOCK_HTTP_H
#define C1_MOCK_HTTP_H

#include "weather/weather_client.h"

void mock_http_set_response(const char *json_body);
void mock_http_set_result(enum HttpResult result);

const char *mock_http_get_last_url(void);
int         mock_http_get_call_count(void);

void mock_http_reset(void);

enum HttpResult mock_fetch(const char *url, char *buf, size_t buf_len);

#endif //C1_MOCK_HTTP_H