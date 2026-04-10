#ifndef C1_GRID_H
#define C1_GRID_H

#include <stddef.h>

#define GRID_MAX_POINTS      64U

#define GRID_FALLBACK_POINTS_COUNT 10U
#define GRID_CITY_NAME_MAX_LEN 64U

struct GridPoint {
    float latitude;
    float longitude;
    char city_name[GRID_CITY_NAME_MAX_LEN];
};

typedef struct {
    struct GridPoint points[GRID_MAX_POINTS];
    size_t           count;
} GridPointArray;

extern const struct GridPoint GRID_FALLBACK_POINTS[GRID_FALLBACK_POINTS_COUNT];

size_t grid_point_count(void);

const struct GridPoint *grid_get_point_from_array(
    const GridPointArray *array,
    size_t index
);

const struct GridPoint *grid_get_fallback_point(size_t index);

#endif
