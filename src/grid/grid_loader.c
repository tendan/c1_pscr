#include "grid_loader.h"
#include "logger/logger.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LINE_BUFFER_SIZE 256U
#define CSV_CITY_MAX_LEN  64U

static int is_comment_or_empty(const char *line)
{
    while (*line && isspace((unsigned char)*line)) {
        line++;
    }
    return (*line == '#' || *line == '\0');
}

static int parse_csv_line(
    const char       *line,
    size_t            line_number,
    struct GridPoint *out)
{
    char  city[CSV_CITY_MAX_LEN] = {0};
    float lat  = 0.0f;
    float lon  = 0.0f;

    /* Format: city,latitude,longitude */
    if (sscanf(line, "%63[^,],%f,%f", city, &lat, &lon) != 3) {
        log_message(LEVEL_WARN, "grid_loader: invalid line %zu — skipping: %s",
            line_number, line);
        return 0;
    }

    /* Walidacja zakresu dla Polski */
    if (lat < 49.0f || lat > 54.9f) {
        log_message(LEVEL_WARN, "grid_loader: line %zu latitude %.2f out of Poland bounds"
            " — skipping",
            line_number, (double)lat);
        return 0;
    }

    if (lon < 14.1f || lon > 24.2f) {
        log_message(LEVEL_WARN, "grid_loader: line %zu longitude %.2f out of Poland bounds"
            " — skipping",
            line_number, (double)lon);
        return 0;
    }

    out->latitude  = lat;
    out->longitude = lon;
    out->city_name = NULL;    /* city_name jest const char* — patrz uwaga */

    /* Skopiuj nazwę miasta do statycznego bufora */
    static char city_names[GRID_MAX_POINTS][CSV_CITY_MAX_LEN];
    static size_t city_index = 0;

    if (city_index < GRID_MAX_POINTS) {
        strncpy(city_names[city_index], city, CSV_CITY_MAX_LEN - 1);
        city_names[city_index][CSV_CITY_MAX_LEN - 1] = '\0';
        out->city_name = city_names[city_index];
        city_index++;
    }

    return 1;
}

GridLoadResult grid_load_from_file(
    const char     *path,
    GridPointArray *out)
{
    if (path == NULL || out == NULL) {
        return GRID_LOAD_ERR_NULL_INPUT;
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        log_message(LEVEL_WARN, "grid_loader: file not found: %s", path);
        return GRID_LOAD_ERR_FILE_NOT_FOUND;
    }

    memset(out, 0, sizeof(GridPointArray));

    char   line[LINE_BUFFER_SIZE];
    size_t line_number = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        line_number++;

        /* Usuń newline */
        line[strcspn(line, "\n")] = '\0';

        if (is_comment_or_empty(line)) {
            continue;
        }

        if (out->count >= GRID_MAX_POINTS) {
            log_message(LEVEL_WARN, "grid_loader: reached max points (%u) — "
                "remaining lines ignored", GRID_MAX_POINTS);
            fclose(f);
            return GRID_LOAD_ERR_TOO_MANY_POINTS;
        }

        if (parse_csv_line(line, line_number, &out->points[out->count])) {
            out->count++;
        }
    }

    fclose(f);

    if (out->count == 0) {
        log_message(LEVEL_ERROR, "grid_loader: no valid points loaded from %s", path);
        return GRID_LOAD_ERR_EMPTY;
    }

    log_message(LEVEL_INFO, "grid_loader: loaded %zu points from %s",
        out->count, path);

    return GRID_LOAD_OK;
}

void grid_load_fallback(GridPointArray *out)
{
    if (out == NULL) {
        return;
    }

    size_t count = grid_point_count();
    if (count > GRID_MAX_POINTS) {
        count = GRID_MAX_POINTS;
    }

    for (size_t i = 0; i < count; i++) {
        out->points[i] = *grid_get_point(i);
    }
    out->count = count;

    log_message(LEVEL_INFO, "grid_loader: using fallback with %zu hardcoded points",
        out->count);
}