/*
        main.c  ~  poem

        the cli frontend, this file shall contain;
        a simple program, it shall be;
        no tui, no interactivity, no fluff;
        neither will it have butts, nor cuts, nor any coconuts.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage() {
    (void)fprintf(
        stderr,
        "Usage: SCB [options] <args>\n"
        "Options:\n"
        "  -h, --help                     Show this help message\n"
        "  -v, --version                  Show version\n"
        "  -c, --compiler <cc>            Explicitly set compiler\n"
        "  -C, --config <path>            Explicitly set config file\n"
        "  -a, --add <dep(s)>             Install a dependency\n"
        "  -r, --remove <dep(s)>          Remove a dependency\n"
        "  -i, --initialize <path>        Initialize a project\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            puts("SCB -- v0.1");
            return EXIT_SUCCESS;
        }

        printf("Unrecognized option: %s\n", argv[i]);
        return EXIT_FAILURE;
    }
}
