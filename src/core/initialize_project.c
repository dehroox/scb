#include "initialize_project.h"
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../main.h"
#include "../util/likely_unlikely.h"

inline int ensure_directory_exists(const char* path) {
    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }
    return 0;
}

inline int init_proj(const char* base_directory_path) {
    const char* effective_base_path =
        base_directory_path ? base_directory_path : ".";
    char directory_buffer[PATH_MAX];

    if (ensure_directory_exists(effective_base_path) == -1) {
        return -1;
    }

    const char* subdirs[] = {"src", "build"};
    for (size_t i = 0; i < sizeof(subdirs) / sizeof(subdirs[0]); i++) {
        (void)snprintf(directory_buffer, sizeof(directory_buffer), "%s/%s",
                       effective_base_path, subdirs[i]);
        if (ensure_directory_exists(directory_buffer) == -1) {
            return -1;
        }
    }

    (void)snprintf(directory_buffer, sizeof(directory_buffer), "%s/%s",
                   effective_base_path, DEFAULT_CONFIG_FILE);
    FILE* config_file = fopen(directory_buffer, "w");
    if (unlikely(!config_file)) {
        perror("fopen");
        return -1;
    }
    (void)fputs(
        "[global]\n"
        "cc = \"gcc\"\n"
        "flags = \"-Wall -Wextra -O2\"\n"
        "files = [\"src/*.c\"]\n"
        "out = \"build/final\"\n\n"
        "[profile.dev]\n"
        "flags = \"-Wall -Wextra -O0 -g3\"\n\n"
        "[profile.release]\n"
        "flags = \"-Wall -Wextra -O3\"\n\n"
        "[package]\n"
        "items = []\n",
        config_file);
    (void)fclose(config_file);

    (void)snprintf(directory_buffer, sizeof(directory_buffer), "%s/src/main.c",
                   effective_base_path);
    FILE* main_file = fopen(directory_buffer, "w");
    if (unlikely(!main_file)) {
        perror("fopen");
        return -1;
    }
    (void)fputs(
        "#include <stdio.h>\n\nint main(void) { puts(\"Hello World\"); }\n",
        main_file);
    (void)fclose(main_file);

    (void)printf("Initialized project at %s\n", effective_base_path);
    return 0;
}
