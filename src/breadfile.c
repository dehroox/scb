/*
    breadfile.c  ~  poem

    A simple file it is, with even simpler contents,
    Buffer-Read-File, a clever name for sure, bread.
    Your utter importance, for everything from IO,
    to anything except parsing, You cover so little,
    yet matter so much.
*/

#include "breadfile.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "likely_unlikely.h"

size_t breadfile(const char* path, void** out) {
    *out = NULL;

    int file_descriptor = open(path, O_RDONLY);
    if (unlikely(file_descriptor < 0)) {
        return 0;
    }

    off_t end = lseek(file_descriptor, 0, SEEK_END);
    if (unlikely(end <= 0)) {
        close(file_descriptor);
        return 0;
    }

    void* buf =
        mmap(NULL, (size_t)end, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    close(file_descriptor);

    if (unlikely(buf == MAP_FAILED)) {
        return 0;
    }

    *out = buf;
    return (size_t)end;
}
