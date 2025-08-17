#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
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

static State state;

static inline void usage(void) {
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

typedef int (*opt_func)(const char* arg);

static inline int handle_help(const char* arg) {
    (void)arg;
    usage();
    exit(EXIT_SUCCESS);
}

static inline int handle_version(const char* arg) {
    (void)arg;
    puts(VERSION);
    exit(EXIT_SUCCESS);
}

static inline int handle_add(const char* arg) {
    printf("Add dependencies: %s\n", arg);
    return 0;
}

static inline int handle_remove(const char* arg) {
    printf("Remove dependencies: %s\n", arg);
    return 0;
}

static inline int handle_init(const char* arg) {
    const char* path = arg ? arg : ".";
    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    char src_dir[PATH_MAX];
    char build_dir[PATH_MAX];
    (void)snprintf(src_dir, sizeof(src_dir), "%s/src", path);
    (void)snprintf(build_dir, sizeof(build_dir), "%s/build", path);

    if (mkdir(src_dir, 0755) == -1 && errno != EEXIST) {
        perror("mkdir src");
        return -1;
    }
    if (mkdir(build_dir, 0755) == -1 && errno != EEXIST) {
        perror("mkdir build");
        return -1;
    }

    char config_path[PATH_MAX];
    (void)snprintf(config_path, sizeof(config_path), "%s/%s", path,
                   DEFAULT_CONFIG_FILE);

    FILE* config_file = fopen(config_path, "w");
    if (!config_file) {
        perror("fopen");
        return -1;
    }
    (void)fprintf(config_file,
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
                  "items = []\n");
    (void)fclose(config_file);

    char mainc_path[PATH_MAX];
    (void)snprintf(mainc_path, sizeof(mainc_path), "%s/%s", src_dir, "main.c");

    FILE* mainc = fopen(mainc_path, "w");
    if (!mainc) {
        perror("fopen");
        return -1;
    }
    (void)fprintf(mainc,
                  "#include <stdio.h>\n"
                  "\n"
                  "int main(void) {\n"
                  "    puts(\"Hello World\");\n"
                  "}\n");
    (void)fclose(mainc);

    printf("Initialized project at %s\n", path);
    return 0;
}

static inline int handle_run(const char* arg) {
    printf("Run profile: %s\n", arg ? arg : "(global)");
    return 0;
}

static inline int handle_build(const char* arg) {
    printf("Build profile: %s\n", arg ? arg : "(global)");
    return 0;
}

typedef struct {
    char opt;
    int requires_arg;  // 1 = required, 0 = optional
    opt_func func;
} Option;

static Option options[] = {
    {'h', 0, handle_help},   {'v', 0, handle_version}, {'a', 1, handle_add},
    {'r', 1, handle_remove}, {'i', 0, handle_init},    {'x', 0, handle_run},
    {'b', 0, handle_build},
};

static inline char* next_arg(int argc, char* argv[], int* index) {
    return (*index + 1 < argc && argv[*index + 1][0] != '-') ? argv[++(*index)]
                                                             : NULL;
}

int main(int argc, char* argv[]) {
    char* config_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'c') {
            if (i + 1 >= argc) {
                (void)fprintf(stderr, "Missing argument for -c\n");
                return EXIT_FAILURE;
            }
            config_file = argv[++i];
        }
    }

    const char* path = config_file ? config_file : DEFAULT_CONFIG_FILE;
    void* buffer = NULL;
    size_t filesize = breadfile(path, &buffer);
    if (unlikely(filesize == 0 || !buffer)) {
        (void)fprintf(stderr, "Failed to read config file: %s\n", path);
        return EXIT_FAILURE;
    }
    state = parset(buffer, (int)filesize);
    munmap(buffer, filesize);

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            continue;
        }
        char opt = argv[i][1];
        char* arg = next_arg(argc, argv, &i);

        int handled = 0;
        for (size_t j = 0; j < sizeof(options) / sizeof(options[0]); j++) {
            handled |= (options[j].opt == opt);
            if (options[j].opt == opt) {
                if (options[j].requires_arg && !arg) {
                    (void)fprintf(stderr, "Missing argument for -%c\n", opt);
                    free_state(&state);
                    return EXIT_FAILURE;
                }
                options[j].func(arg);
                break;
            }
        }

        if (!handled) {
            (void)fprintf(stderr, "Unknown option: -%c\n", opt);
            usage();
            free_state(&state);
            return EXIT_FAILURE;
        }
    }

    free_state(&state);
    return EXIT_SUCCESS;
}
