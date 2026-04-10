#ifndef C1_GRID_LOADER_H
#define C1_GRID_LOADER_H

#include "grid/grid.h"
#include <stddef.h>

#define GRID_MAX_POINTS      64U
#define DEFAULT_GRID_PATH    "/etc/c1/grid.csv"

typedef enum {
    GRID_LOAD_OK = 0,
    GRID_LOAD_ERR_NULL_INPUT,
    GRID_LOAD_ERR_FILE_NOT_FOUND,
    GRID_LOAD_ERR_EMPTY,
    GRID_LOAD_ERR_TOO_MANY_POINTS,
} GridLoadResult;

typedef struct {
    struct GridPoint points[GRID_MAX_POINTS];
    size_t           count;
} GridPointArray;

GridLoadResult grid_load_from_file(
    const char   *path,
    GridPointArray *out
);

void grid_load_fallback(GridPointArray *out);

#endif