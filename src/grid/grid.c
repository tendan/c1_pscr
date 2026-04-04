
#include "grid/grid.h"

const struct GridPoint GRID_POINTS[GRID_POINT_COUNT] = {
    { 53.43f, 14.55f, "Szczecin"     },
    { 54.35f, 18.64f, "Gdansk"       },
    { 54.10f, 22.93f, "Suwalki"      },
    { 51.94f, 15.51f, "Zielona Gora" },
    { 52.41f, 16.93f, "Poznan"       },
    { 52.23f, 21.01f, "Warszawa"     },
    { 51.25f, 22.57f, "Lublin"       },
    { 51.11f, 17.04f, "Wroclaw"      },
    { 50.06f, 19.94f, "Krakow"       },
    { 50.04f, 22.00f, "Rzeszow"      },
};

size_t grid_point_count(void)
{
    return GRID_POINT_COUNT;
}

const struct GridPoint *grid_get_point(size_t index)
{
    if (index >= GRID_POINT_COUNT) {
        return NULL;
    }
    return &GRID_POINTS[index];
}