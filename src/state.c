#include "state.h"
#include <stdlib.h>

void print_state(State* state) {
    if (!state) {
        return;
    }

    printf("Global:\n");
    printf("  CC: %s\n", state->global.cc);
    printf("  Flags: %s\n", state->global.flags);
    printf("  Out: %s\n", state->global.out);
    printf("  Files (%zu):\n", state->global.file_num);
    for (size_t i = 0; i < state->global.file_num; i++) {
        printf("    %s\n", state->global.files[i]);
    }

    printf("\nProfiles (%zu):\n", state->profiles_num);
    for (size_t i = 0; i < state->profiles_num; i++) {
        const Profile* profile = &state->profiles[i];
        printf("  [%s]\n", profile->name);
        printf("    CC: %s\n", profile->cc);
        printf("    Flags: %s\n", profile->flags);
        printf("    Out: %s\n", profile->out);
        printf("    Files (%zu):\n", profile->file_num);
        for (size_t j = 0; j < profile->file_num; j++) {
            printf("      %s\n", profile->files[j]);
        }
    }

    printf("\nPackages (%zu):\n", state->package_num);
    for (size_t i = 0; i < state->package_num; i++) {
        const Package* pkg = &state->packages[i];
        printf("  Name: %s, Version: %s\n", pkg->name, pkg->version);
    }
}

void free_state(State* state) {
    if (!state) {
        return;
    }

    if (state->global.files) {
        free((void*)state->global.files);
    }

    for (size_t i = 0; i < state->profiles_num; i++) {
        Profile* profile = &state->profiles[i];
        if (profile->files != state->global.files && profile->files) {
            free((void*)profile->files);
        }
    }

    if (state->profiles) {
        free(state->profiles);
    }
    if (state->packages) {
        free(state->packages);
    }

    toml_free((state->toml_res));

    state->profiles = NULL;
    state->packages = NULL;
    state->global.files = NULL;
    state->profiles_num = 0;
    state->package_num = 0;
}
