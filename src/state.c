#include "state.h"
#include <stdlib.h>
#include "likely_unlikely.h"

void print_state(const State* state) {
    if (unlikely(!state)) {
        return;
    }

    printf("=== Global Profile ===\n");
    printf("CC: %s\nFlags: %s\nOut: %s\nFiles:", state->global.cc,
           state->global.flags, state->global.out);

    for (size_t file_index = 0; file_index < state->global.file_num;
         file_index++) {
        printf(" %s", state->global.files[file_index]);
    }
    printf("\n\n");

    printf("=== Profiles ===\n");
    for (size_t prof_index = 0; prof_index < state->profiles_num;
         prof_index++) {
        Profile* current_profile = &state->profiles[prof_index];
        printf("[%s]\n", current_profile->name);
        printf("CC: %s\nFlags: %s\nOut: %s\nFiles:", current_profile->cc,
               current_profile->flags, current_profile->out);

        for (size_t file_index = 0; file_index < current_profile->file_num;
             file_index++) {
            printf(" %s", current_profile->files[file_index]);
        }
        printf("\n\n");
    }

    printf("=== Packages ===\n");
    for (size_t pkg_index = 0; pkg_index < state->package_num; pkg_index++) {
        Package* current_package = &state->packages[pkg_index];
        printf("%s: %s\n", current_package->name, current_package->version);
    }
}

void free_state(State* state) {
    if (unlikely(!state)) {
        return;
    }

    for (size_t prof_index = 0; prof_index < state->profiles_num;
         prof_index++) {
        Profile* current_profile = &state->profiles[prof_index];
        if (current_profile->files &&
            current_profile->files != state->global.files) {
            free((void*)current_profile->files);
        }
    }

    free(state->profiles);

    if (likely(state->global.files)) {
        free((void*)state->global.files);
    }

    if (likely(state->packages)) {
        free(state->packages);
    }

    if (likely(state->toml_res.ok)) {
        toml_free(state->toml_res);
    }

    *state = (State){0};
}
