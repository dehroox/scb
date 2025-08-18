#ifndef SORTS_H
#define SORTS_H

#include <stddef.h>
#include <unistd.h>
typedef int (*CompareFunc)(const void* vara, const void* varb);

ssize_t binary_search(void* arr,
                      int element_size,
                      const char* target,
                      size_t size,
                      CompareFunc compare);

#endif
