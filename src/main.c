/*
    main.c  ~  poem

    the cli frontend, this file shall contain;
    a simple program, it shall be,
    no tui, no interactivity, no fluff,
    neither will it have butts, nor cuts, nor any coconuts.
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <yaml.h>
#include "../include/breadfile.h"
#include "../include/likely_unlikely.h"

#define VERSION "0.1"
#define DEFAULT_CONFIG_FILE "scb.yml"

static inline void usage() {
    (void)fprintf(stderr,
                  "Usage: SCB [options] <args>\n"
                  "Options:\n"
                  "  -h                   Show this help message\n"
                  "  -v                   Show version\n"
                  "  -c <path>            Explicitly set config file\n"
                  "  -a <dep(s)>          Add a dependency\n"
                  "  -r <dep(s)>          Remove a dependency\n"
                  "  -i <?path>           Initialize a project\n"
                  "  -r <?profile>        Run default/specified profile\n"
                  "  -b <?profile>        Build default/specified profile\n");
}

int main(int argc, char* argv[]) {
    char* config_file = NULL;

    if (argc < 2 || argv[1][0] != '-') {
        usage();
        return EXIT_FAILURE;
    }

    char opt = argv[1][1];

    switch (opt) {
        case 'h':
            usage();
            return EXIT_SUCCESS;

        case 'v':
            puts(VERSION);
            return EXIT_SUCCESS;

        case 'c':
            if (argc < 3) {
                (void)fprintf(stderr, "Missing argument for -c\n");
                return EXIT_FAILURE;
            }
            config_file = argv[2];
            break;

        default:
            (void)fprintf(stderr, "Unrecognized option: %s\n", argv[1]);
            usage();
            return EXIT_FAILURE;
    }

    const char* path = config_file ? config_file : DEFAULT_CONFIG_FILE;

    void* buffer = NULL;
    size_t filesize = breadfile(path, &buffer);

    if (unlikely(filesize == 0 || buffer == NULL)) {
        (void)fprintf(stderr, "Failed to read config file: %s\n", path);
        return EXIT_FAILURE;
    }

    munmap(buffer, filesize);
    return EXIT_SUCCESS;
}
