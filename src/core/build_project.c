#include "build_project.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../config/config.h"
#include "../main.h"
#include "../util/comparators.h"
#include "../util/sorts.h"

int build_project(const char* argument, Config* config) {
    Profile profile = {0};

    if (strncmp(argument, "global", 6) == 0) {
        profile = config->global;
    } else {
        ssize_t search_result =
            binary_search(config->profiles, sizeof(Profile), argument,
                          config->profiles_num, compare_profile_names);

        if (search_result != -1) {
            profile = config->profiles[search_result];
        }
    }

    if (!!profile.name) {
        printf("Building profile: %s\n", profile.name);
    } else {
        printf("Profile not found or invalid\n");
        return -1;
    }

    const char** argv = (const char**)malloc(
        sizeof(char*) * (profile.file_num + profile.flag_num + 4));
    argv[0] = profile.cc;
    argv[1] = "-o";
    argv[2] = profile.out;
    for (size_t j = 0; j < profile.flag_num; j++) {
        argv[3 + profile.file_num + j] = profile.flags[j];
    }

    for (size_t i = 0; i < profile.file_num; i++) {
        argv[3 + i] = profile.files[i];
    }

    argv[3 + profile.file_num + profile.flag_num] = NULL;

    PRINT_ARRAY(argv, profile.file_num + profile.flag_num + 3, "%s");
    
    return execvp(profile.cc, (char* const*)argv);
}
