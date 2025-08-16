#ifndef READFILE_H
#define READFILE_H

#include <stddef.h>

/*
    Reads file into `out`, returns 0 if failed, returns the filesize when it
    succeeds.
*/
size_t breadfile(const char* path, void** out);
int breadfile_free(void* buf, size_t filesize);

#endif
