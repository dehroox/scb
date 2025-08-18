#include "build_project.h"
#include <stdio.h>
#include <string.h>
#include "../config/config.h"
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

    return 0;
}
