#include "load_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "../config/config.h"
#include "../config/parse_config.h"
#include "../util/likely_unlikely.h"
#include "../util/read_file.h"

inline int load_config(const char* path, Config* out_state) {
    void* buffer = NULL;
    size_t size = read_file(path, &buffer);
    if (unlikely(size == 0 || !buffer)) {
        (void)fprintf(stderr, "Failed to read config file: %s\n", path);
        return -1;
    }

    *out_state = parse_config(buffer, (int)size);
    if (unlikely(!out_state->toml_res.ok)) {
        (void)fprintf(stderr, "Failed to parse config file\n");
        (void)munmap(buffer, size);
        return -1;
    }

    (void)munmap(buffer, size);
    return 0;
}
