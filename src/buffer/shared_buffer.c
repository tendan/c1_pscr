#include "shared_buffer.h"

#include <string.h>
#include <errno.h>
#include <time.h>

static void make_timeout(struct timespec *ts)
{
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += SHARED_BUFFER_TIMEOUT_S;
}

BufferResult shared_buffer_init(SharedBuffer *buf)
{
    if (buf == NULL) {
        return BUFFER_ERR_NULL_INPUT;
    }

    memset(buf, 0, sizeof(SharedBuffer));

    if (sem_init(&buf->sem_produced, 0, 0) != 0) {
        return BUFFER_ERR_SEM_INIT_FAILED;
    }

    if (sem_init(&buf->sem_consumed, 0, 1) != 0) {
        sem_destroy(&buf->sem_produced);
        return BUFFER_ERR_SEM_INIT_FAILED;
    }

    if (pthread_mutex_init(&buf->mutex, NULL) != 0) {
        sem_destroy(&buf->sem_produced);
        sem_destroy(&buf->sem_consumed);
        return BUFFER_ERR_SEM_INIT_FAILED;
    }

    return BUFFER_OK;
}

void shared_buffer_destroy(SharedBuffer *buf)
{
    if (buf == NULL) {
        return;
    }

    sem_destroy(&buf->sem_produced);
    sem_destroy(&buf->sem_consumed);
    pthread_mutex_destroy(&buf->mutex);
    memset(buf, 0, sizeof(SharedBuffer));
}

BufferResult shared_buffer_produce(
    SharedBuffer *buf,
    const struct CalculatedWeatherData *data,
    int valid,
    enum WeatherDataResult status)
{
    if (buf == NULL || data == NULL) {
        return BUFFER_ERR_NULL_INPUT;
    }

    struct timespec ts;
    make_timeout(&ts);

    if (sem_timedwait(&buf->sem_consumed, &ts) != 0) {
        return (errno == ETIMEDOUT)
                   ? BUFFER_ERR_TIMEOUT
                   : BUFFER_ERR_INVALID_STATE;
    }

    pthread_mutex_lock(&buf->mutex);
    memcpy(&buf->data, data, sizeof(struct CalculatedWeatherData));
    buf->valid = valid;
    buf->status = status;
    pthread_mutex_unlock(&buf->mutex);

    sem_post(&buf->sem_produced);
    return BUFFER_OK;
}

BufferResult shared_buffer_consume(
    SharedBuffer *buf,
    struct CalculatedWeatherData *out,
    int *out_valid,
    enum WeatherDataResult *out_status)
{
    if (buf == NULL || out == NULL ||
        out_valid == NULL || out_status == NULL) {
        return BUFFER_ERR_NULL_INPUT;
    }

    struct timespec ts;
    make_timeout(&ts);

    if (sem_timedwait(&buf->sem_produced, &ts) != 0) {
        return (errno == ETIMEDOUT)
                   ? BUFFER_ERR_TIMEOUT
                   : BUFFER_ERR_INVALID_STATE;
    }

    pthread_mutex_lock(&buf->mutex);
    memcpy(out, &buf->data, sizeof(struct CalculatedWeatherData));
    *out_valid = buf->valid;
    *out_status = buf->status;
    buf->valid = 0;
    pthread_mutex_unlock(&buf->mutex);

    sem_post(&buf->sem_consumed);
    return BUFFER_OK;
}
