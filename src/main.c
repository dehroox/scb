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
#define DEFAULT_CONFIG_FILE "/scb.yml"

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
    char cwd[1024];
    char* config_file = NULL;

    if (unlikely(!getcwd(cwd, sizeof cwd))) {
        (void)fprintf(stderr, "Failed to get CWD, exiting.");
        return EXIT_FAILURE;
    }

    int arg = (argc > 1) ? argv[1][1] : '?';

    switch (arg) {
        case 'h':
            usage();
            break;

        case 'v':
            puts(VERSION);
            break;

        case 'c':
            if (!argv[2]) {
                (void)fprintf(stderr,
                              "Missing positional argument for config file\n");
                return EXIT_FAILURE;
            }

            config_file = argv[2];
            break;

        default:
            (void)fprintf(stderr, "Unrecognized option: %s\n", argv[1]);
            return EXIT_FAILURE;
    }

    void* buffer = NULL;
    const char* cpath =
        strncat(cwd, config_file ? config_file : DEFAULT_CONFIG_FILE,
                sizeof(cwd) - strlen(cwd) - 1);

    size_t filesize = breadfile(cpath, &buffer);

    if (unlikely(filesize == 0) || unlikely(buffer == NULL)) {
        (void)fprintf(stderr, "Failed to read config file, exiting..");
        return EXIT_FAILURE;
    }

    munmap(buffer, filesize);
    return EXIT_SUCCESS;
}
