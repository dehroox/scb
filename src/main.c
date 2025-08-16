/*
        main.c  ~  poem

        the cli frontend, this file shall contain;
        a simple program, it shall be;
        no tui, no interactivity, no fluff;
        neither will it have butts, nor cuts, nor any coconuts.
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VERSION "0.1"

static void usage() {
    (void)fprintf(
        stderr,
        "Usage: SCB [options] <args>\n"
        "Options:\n"
        "  -h                   Show this help message\n"
        "  -v                   Show version\n"
        "  -c <path>            Explicitly set config file\n"
        "  -a <dep(s)>          Add a dependency\n"
        "  -r <dep(s)>          Remove a dependency\n"
        "  -i <path>            Initialize a project\n"
        "  -r <?profile>        Run default or a specified profile\n"
        "  -b <?profile>        Build default or a specified profile\n");
}

int main(int argc, char* argv[]) {
    char cwd[1024];
    char* config_file = NULL;

    (void)getcwd(cwd, sizeof cwd);

    char arg = (argc > 1) ? argv[1][1] : '?';

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

    puts(config_file ? config_file : "scb.yml");
    return EXIT_SUCCESS;
}
