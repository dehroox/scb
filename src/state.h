#ifndef STATE_H
#define STATE_H

typedef struct {
    char* cc;
    char* flags;
    char* entry;
    char* out;
} Profile;

typedef struct {
    Profile global;
    Profile selected;
} State;

#endif
