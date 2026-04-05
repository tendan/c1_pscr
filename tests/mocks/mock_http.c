//
// Created by tendan on 31.03.2026.
//

#include "mock_http.h"

#include <pthread.h>
#include <string.h>
#include <bits/pthreadtypes.h>

static char s_response[4096];
static enum HttpResult s_result;
static char s_last_url[512];
static int s_call_count;
static pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;

void mock_http_set_response(const char *json_body)
{
    strncpy(s_response, json_body, sizeof(s_response) - 1);
}

void mock_http_set_result(enum HttpResult result)
{
    s_result = result;
}

const char *mock_http_get_last_url(void)
{
    return s_last_url;
}

int mock_http_get_call_count(void)
{
    return s_call_count;
}

void mock_http_reset(void)
{
    memset(s_response, 0, sizeof(s_response));
    memset(s_last_url, 0, sizeof(s_last_url));
    s_result = HTTP_OK;
    s_call_count = 0;
}

enum HttpResult mock_fetch(const char *url, char *buf, size_t buf_len)
{
    pthread_mutex_lock(&s_mutex);
    strncpy(s_last_url, url, sizeof(s_last_url) - 1);
    s_call_count++;
    pthread_mutex_unlock(&s_mutex);

    if (s_result == HTTP_OK) {
        strncpy(buf, s_response, buf_len - 1);
    }

    return s_result;
}
