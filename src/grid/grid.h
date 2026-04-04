#ifndef C1_GRID_H
#define C1_GRID_H

#include <stddef.h>

#define GRID_POINT_COUNT 10U

struct GridPoint {
    float latitude;
    float longitude;
    const char *city_name;    /* czytelność logów i debugowania */
};

extern const struct GridPoint GRID_POINTS[GRID_POINT_COUNT];

size_t grid_point_count(void);

/* Zwraca wskaźnik na punkt lub NULL gdy index poza zakresem */
const struct GridPoint *grid_get_point(size_t index);

#endif