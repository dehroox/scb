#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "cli/cli.h"
#include "cli/load_config.h"
#include "config/config.h"
#include "util/likely_unlikely.h"

static Config loaded_config;

int main(int argc, char* argv[]) {
    char* explicit_config_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'c') {
            if (unlikely(i + 1 >= argc)) {
                (void)fprintf(stderr, "Missing argument for -c\n");
                return EXIT_FAILURE;
            }
            explicit_config_path = argv[++i];
        }
    }

    const char* config_path =
        explicit_config_path ? explicit_config_path : DEFAULT_CONFIG_FILE;

    if (load_config(config_path, &loaded_config) != 0) {
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            continue;
        }
        char option_character = argv[i][1];
        char* option_argument = fetch_next_argument(argc, argv, &i);
        int status = get_opt(option_character, option_argument, &loaded_config);
        if (status != -1) {
            return status;
        }
    }
    free_config(&loaded_config);
    return EXIT_SUCCESS;
}
