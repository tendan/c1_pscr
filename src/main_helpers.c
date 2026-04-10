
#include <unistd.h>

const char *resolve_grid_path(
    const char *cli_path,
    const char *default_path)
{
    if (cli_path != NULL) {
        return cli_path;
    }
    if (access(default_path, R_OK) == 0) {
        return default_path;
    }
    return NULL;
}
