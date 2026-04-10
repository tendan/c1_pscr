#ifndef C1_GRID_H
#define C1_GRID_H

#include <stddef.h>

#define GRID_POINT_COUNT 10U
#define GRID_CITY_NAME_MAX_LEN 64U

struct GridPoint {
    float latitude;
    float longitude;
    char city_name[GRID_CITY_NAME_MAX_LEN];
};

extern const struct GridPoint GRID_POINTS[GRID_POINT_COUNT];

size_t grid_point_count(void);

/* Zwraca wskaźnik na punkt lub NULL gdy index poza zakresem */
const struct GridPoint *grid_get_point(size_t index);

#endif