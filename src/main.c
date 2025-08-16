/*
        main.c  ~  poem

        the cli frontend, this file shall contain;
        a simple program, it shall be;
        no tui, no interactivity, no fluff;
        neither will it have butts, nor cuts, nor any coconuts.
*/

#include <stdio.h>
#include <stdlib.h>

static void usage() {
    (void)fprintf(stderr,
                  "Usage: SCB [options] <args>\n"
                  "Options:\n"
                  "  -h                   Show this help message\n"
                  "  -v                   Show version\n"
                  "  -c <cc>              Explicitly set compiler\n"
                  "  -C <path>            Explicitly set config file\n"
                  "  -a <dep(s)>          Install a dependency\n"
                  "  -r <dep(s)>          Remove a dependency\n"
                  "  -i <path>            Initialize a project\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return EXIT_FAILURE;
    }

    char arg = argv[1][1];

    switch (arg) {
        case 'h':
            usage();
            return EXIT_SUCCESS;

        default:
            (void)fprintf(stderr, "Unrecognized option: %s\n", argv[1]);
            return EXIT_FAILURE;
    }
}
