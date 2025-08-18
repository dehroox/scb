#include "sorts.h"

#include <stddef.h>
#include <unistd.h>
#include "../config/config.h"

ssize_t binary_search(void* arr,
                      int element_size,
                      const char* target,
                      size_t size,
                      CompareFunc compare) {
    if (!arr || !target || !compare || size <= 0) {
        return -1;
    }

    size_t left = 0;
    size_t right = size - 1;

    while (left <= right) {
        size_t mid = left + ((right - left) / 2);

        void* midElement = (char*)arr + ((ptrdiff_t)mid * element_size);
        Profile temp_profile = {0};
        temp_profile.name = target;

        int comparison = compare(midElement, &temp_profile);

        if (comparison == 0) {
            return (ssize_t)mid;
        }
        if (comparison < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}
