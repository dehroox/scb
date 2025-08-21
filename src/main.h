#ifndef MAIN_H
#define MAIN_H

#define VERSION "0.1"
#define MAX_PROFILE_NAME_SIZE 128
#define DEFAULT_CONFIG_FILE "scb.toml"

#define PRINT_ARRAY(arr, len, fmt)                                    \
    do {                                                              \
        for (size_t i = 0; i < (len); i++) {                          \
            printf(fmt "%s", (arr)[i], (i + 1 == (len)) ? "" : ", "); \
        }                                                             \
        puts("");                                                     \
    } while (0)

#endif
