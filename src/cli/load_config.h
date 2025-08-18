#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stddef.h>
#include "../config/config.h"

int load_config(const char* path, Config* out_state);

#endif
