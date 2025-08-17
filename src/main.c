#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "breadfile.h"
#include "likely_unlikely.h"
#include "parset.h"
#include "state.h"

#define VERSION "0.1"
#define DEFAULT_CONFIG_FILE "scb.toml"

static State global_state;

static inline void print_usage(void) {
    (void)fputs(
        "Usage: SCB [options]\n"
        "Options:\n"
        "  -h                   Show this help message\n"
        "  -v                   Show version\n"
        "  -c <path>            Explicitly set config file\n"
        "  -a <dep(s)>          Add a dependency\n"
        "  -r <dep(s)>          Remove a dependency\n"
        "  -i <?path>           Initialize a project\n"
        "  -x <?profile>        Run default/specified profile\n"
        "  -b <?profile>        Build default/specified profile\n",
        stderr);
}

static inline int print_action(const char* action_label,
                               const char* argument,
                               const char* fallback) {
    (void)printf("%s: %s\n", action_label, argument ? argument : fallback);
    return 0;
}

static inline int ensure_directory_exists(const char* path) {
    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }
    return 0;
}

static inline int option_init_handler(const char* base_directory_path) {
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

static inline char* fetch_next_argument(int argc,
                                        char* argv[],
                                        int* index_ptr) {
    return (*index_ptr + 1 < argc && argv[*index_ptr + 1][0] != '-')
               ? argv[++(*index_ptr)]
               : NULL;
}

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

    const char* effective_config_path =
        explicit_config_path ? explicit_config_path : DEFAULT_CONFIG_FILE;
    void* config_file_buffer = NULL;
    size_t config_file_size =
        breadfile(effective_config_path, &config_file_buffer);
    if (unlikely(config_file_size == 0 || !config_file_buffer)) {
        (void)fprintf(stderr, "Failed to read config file: %s\n",
                      effective_config_path);
        return EXIT_FAILURE;
    }
    global_state = parset(config_file_buffer, (int)config_file_size);
    (void)munmap(config_file_buffer, config_file_size);

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            continue;
        }

        char option_character = argv[i][1];
        char* option_argument = fetch_next_argument(argc, argv, &i);

        switch (option_character) {
            case 'h':
                print_usage();
                free_state(&global_state);
                return EXIT_SUCCESS;
            case 'v':
                (void)puts(VERSION);
                free_state(&global_state);
                return EXIT_SUCCESS;
            case 'a':
                if (unlikely(!option_argument)) {
                    (void)fprintf(stderr, "Missing argument for -a\n");
                    free_state(&global_state);
                    return EXIT_FAILURE;
                }
                (void)print_action("Add dependencies", option_argument, NULL);
                break;
            case 'r':
                if (unlikely(!option_argument)) {
                    (void)fprintf(stderr, "Missing argument for -r\n");
                    free_state(&global_state);
                    return EXIT_FAILURE;
                }
                (void)print_action("Remove dependencies", option_argument,
                                   NULL);
                break;
            case 'i':
                (void)option_init_handler(option_argument);
                break;
            case 'x':
                (void)print_action("Run profile", option_argument, "(global)");
                break;
            case 'b':
                (void)print_action("Build profile", option_argument,
                                   "(global)");
                break;
            default:
                (void)fprintf(stderr, "Unknown option: -%c\n",
                              option_character);
                print_usage();
                free_state(&global_state);
                return EXIT_FAILURE;
        }
    }

    free_state(&global_state);
    return EXIT_SUCCESS;
}
