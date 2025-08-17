#include "parset.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "likely_unlikely.h"
#include "state.h"
#include "tomlc17.h"

static inline const char** build_file_array(toml_datum_t arr_table,
                                            size_t* out_count) {
    size_t count = (size_t)arr_table.u.arr.size;
    *out_count = count;
    if (unlikely(count == 0)) {
        return NULL;
    }
    const char** files = (const char**)malloc(sizeof(char*) * count);
    for (size_t idx = 0; idx < count; idx++) {
        files[idx] = arr_table.u.arr.elem[idx].u.s;
    }
    return files;
}

State parset(const char* src, int len) {
    State parsed_state = {0};

    parsed_state.toml_res = toml_parse(src, len);
    if (unlikely(!parsed_state.toml_res.ok)) {
        (void)fprintf(stderr, "TOML parse failed: %s\n",
                      parsed_state.toml_res.errmsg);
        parsed_state.toml_res.ok = false;
        return parsed_state;
    }

    toml_datum_t top_table = parsed_state.toml_res.toptab;

    toml_datum_t global_table = toml_get(top_table, "global");
    parsed_state.global.name = "global";
    parsed_state.global.cc = toml_get(global_table, "cc").u.s;
    parsed_state.global.flags = toml_get(global_table, "flags").u.s;
    parsed_state.global.out = toml_get(global_table, "out").u.s;

    toml_datum_t global_files_array = toml_get(global_table, "files");
    parsed_state.global.files =
        build_file_array(global_files_array, &parsed_state.global.file_num);

    toml_datum_t profile_parent_table = toml_get(top_table, "profile");
    parsed_state.profiles =
        malloc(sizeof(Profile) * (size_t)profile_parent_table.u.tab.size);
    parsed_state.profiles_num = 0;

    for (size_t key_index = 0;
         key_index < (size_t)profile_parent_table.u.tab.size; key_index++) {
        const char* profile_name = profile_parent_table.u.tab.key[key_index];
        toml_datum_t profile_table =
            profile_parent_table.u.tab.value[key_index];

        Profile* current_profile =
            &parsed_state.profiles[parsed_state.profiles_num++];
        current_profile->name = profile_name;
        current_profile->cc = parsed_state.global.cc;
        current_profile->flags = parsed_state.global.flags;
        current_profile->out = parsed_state.global.out;
        current_profile->files = parsed_state.global.files;
        current_profile->file_num = parsed_state.global.file_num;

        toml_datum_t val;

        val = toml_get(profile_table, "cc");
        current_profile->cc =
            val.type == TOML_STRING ? val.u.s : current_profile->cc;

        val = toml_get(profile_table, "flags");
        current_profile->flags =
            val.type == TOML_STRING ? val.u.s : current_profile->flags;

        val = toml_get(profile_table, "out");
        current_profile->out =
            val.type == TOML_STRING ? val.u.s : current_profile->out;

        val = toml_get(profile_table, "files");
        if (likely(val.type == TOML_ARRAY)) {
            current_profile->file_num = (size_t)val.u.arr.size;
            current_profile->files =
                build_file_array(val, &current_profile->file_num);
        }
    }

    toml_datum_t package_table = toml_get(top_table, "package");
    toml_datum_t package_items = toml_get(package_table, "items");
    parsed_state.package_num = (size_t)package_items.u.arr.size;
    parsed_state.packages =
        parsed_state.package_num
            ? malloc(sizeof(Package) * parsed_state.package_num)
            : NULL;

    for (size_t pkg_index = 0; pkg_index < parsed_state.package_num;
         pkg_index++) {
        toml_datum_t pkg_table = package_items.u.arr.elem[pkg_index];
        parsed_state.packages[pkg_index].name = toml_get(pkg_table, "name").u.s;
        parsed_state.packages[pkg_index].version =
            toml_get(pkg_table, "version").u.s;
    }

    return parsed_state;
}
