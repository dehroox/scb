#include "cli.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../config/config.h"
#include "../core/build_project.h"
#include "../core/initialize_project.h"
#include "../main.h"
#include "../util/likely_unlikely.h"

inline void print_usage(void) {
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

inline int print_action(const char* action_label,
                        const char* argument,
                        const char* fallback) {
    (void)printf("%s: %s\n", action_label, argument ? argument : fallback);
    return 0;
}

inline char* fetch_next_argument(int argc, char* argv[], int* index_ptr) {
    return (*index_ptr + 1 < argc && argv[*index_ptr + 1][0] != '-')
               ? argv[++(*index_ptr)]
               : NULL;
}

inline int get_opt(char option_character,
                   char* option_argument,
                   Config* global_config) {
    switch (option_character) {
        case 'h':
            print_usage();
            break;
        case 'v':
            (void)puts(VERSION);
            break;
        case 'a':
            if (unlikely(!option_argument)) {
                (void)fprintf(stderr, "Missing argument for -a\n");
                free_config(global_config);
                return EXIT_FAILURE;
            }
            (void)print_action("Add dependencies", option_argument, NULL);
            break;
        case 'r':
            if (unlikely(!option_argument)) {
                (void)fprintf(stderr, "Missing argument for -r\n");
                free_config(global_config);
                return EXIT_FAILURE;
            }
            (void)print_action("Remove dependencies", option_argument, NULL);
            break;
        case 'i':
            (void)init_proj(option_argument);
            break;
        case 'x':
            (void)print_action("Run profile", option_argument, "(global)");
            break;
        case 'b':
            (void)build_project(option_argument ? option_argument : "global",
                                global_config);
            break;
        default:
            (void)fprintf(stderr, "Unknown option: -%c\n", option_character);
            print_usage();
            free_config(global_config);
            return EXIT_FAILURE;
    }
    return -1;
}
