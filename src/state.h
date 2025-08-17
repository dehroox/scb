#ifndef STATE_H
#define STATE_H

#include <stddef.h>
#include "tomlc17.h"
typedef struct {
    const char* name;
    const char* cc;
    const char* flags;
    size_t file_num;
    const char** files;
    const char* out;
} Profile;

typedef struct {
    const char* name;
    const char* version;
} Package;

typedef struct {
    Profile global;
    size_t profiles_num;
    Profile* profiles;
    size_t package_num;
    Package* packages;
    toml_result_t toml_res;
} State;

void print_state(State* state);
void free_state(State* state);

#endif
