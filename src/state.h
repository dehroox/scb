#ifndef STATE_H
#define STATE_H

#include <stddef.h>
typedef struct {
    const char* cc;
    const char* flags;
    const size_t file_num;
    const char** files;
    const char* out;
} Profile;

typedef struct {
    const char* name;
    const char* version;
} Package;

typedef struct {
    Profile global;
    Profile selected;
    size_t package_num;
    Package* packages;
} State;

#endif
