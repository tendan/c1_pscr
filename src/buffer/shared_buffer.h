#ifndef C1_SHARED_BUFFER_H
#define C1_SHARED_BUFFER_H

#include <semaphore.h>
#include <pthread.h>
#include "weather/weather_transformer.h"

#define SHARED_BUFFER_TIMEOUT_S 5

typedef enum {
    BUFFER_OK = 0,
    BUFFER_ERR_NULL_INPUT,
    BUFFER_ERR_TIMEOUT,
    BUFFER_ERR_SEM_INIT_FAILED,
    BUFFER_ERR_INVALID_STATE,
} BufferResult;

typedef struct {
    struct CalculatedWeatherData data;
    int                          valid;
    sem_t                        sem_produced;
    sem_t                        sem_consumed;
    pthread_mutex_t              mutex;
} SharedBuffer;

BufferResult shared_buffer_init(SharedBuffer *buf);
void         shared_buffer_destroy(SharedBuffer *buf);

BufferResult shared_buffer_produce(
    SharedBuffer                       *buf,
    const struct CalculatedWeatherData *data
);

BufferResult shared_buffer_consume(
    SharedBuffer               *buf,
    struct CalculatedWeatherData *out
);

#endif