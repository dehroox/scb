#ifndef READFILE_H
#define READFILE_H

#include <stddef.h>

/*
    Reads file into `out`, returns 0 if failed, returns the filesize when it
    succeeds.

    DO NOT free() THE BUFFER! USE munmap() instead.
*/
size_t breadfile(const char* path, void** out);

#endif
