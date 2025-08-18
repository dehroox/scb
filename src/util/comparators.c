#include "comparators.h"
#include <string.h>
#include "../config/config.h"

int compare_profile_names(const void* vara, const void* varb) {
    const Profile* prof1 = (const Profile*)vara;
    const Profile* prof2 = (const Profile*)varb;

    return strcmp((prof1)->name, (prof2)->name);
}
