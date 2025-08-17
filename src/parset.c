#include "parset.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "tomlc17.h"

State parset(const char* src, int len) {
    toml_result_t res = toml_parse(src, len);
    if (!res.ok) {
        (void)fprintf(stderr, "TOML parse failed: %s\n", res.errmsg);
        exit(1);
    }

    toml_datum_t top = res.toptab;

    toml_datum_t gtab = toml_get(top, "global");
    Profile gbl = {0};
    gbl.name = "global";
    gbl.cc = toml_get(gtab, "cc").u.s;
    gbl.flags = toml_get(gtab, "flags").u.s;
    gbl.out = toml_get(gtab, "out").u.s;

    toml_datum_t arr = toml_get(gtab, "files");
    gbl.file_num = (size_t)arr.u.arr.size;
    gbl.files =
        (const char**)(arr.u.arr.elem ? malloc(sizeof(char*) * gbl.file_num)
                                      : NULL);
    for (size_t i = 0; i < gbl.file_num; i++) {
        gbl.files[i] = arr.u.arr.elem[i].u.s;
    }

    size_t nkeys = (size_t)top.u.tab.size;
    Profile* profs = malloc(sizeof(Profile) * nkeys);
    size_t prof_count = 0;

    for (size_t i = 0; i < nkeys; i++) {
        const char* kch = top.u.tab.key[i];
        if (strncmp(kch, "profile.", 8) != 0) {
            continue;
        }

        toml_datum_t profile_table = top.u.tab.value[i];
        Profile* profile = &profs[prof_count];
        profile->name = kch + 8; 

        toml_datum_t val;

        val = toml_get(profile_table, "cc");
        profile->cc = val.type == TOML_STRING ? val.u.s : gbl.cc;

        val = toml_get(profile_table, "flags");
        profile->flags = val.type == TOML_STRING ? val.u.s : gbl.flags;

        val = toml_get(profile_table, "out");
        profile->out = val.type == TOML_STRING ? val.u.s : gbl.out;

        val = toml_get(profile_table, "files");
        if (val.type == TOML_ARRAY) {
            profile->file_num = (size_t)val.u.arr.size;
            profile->files =
                (const char**)malloc(sizeof(char*) * profile->file_num);
            for (size_t j = 0; j < profile->file_num; j++) {
                profile->files[j] = val.u.arr.elem[j].u.s;
            }
        } else {
            profile->file_num = gbl.file_num;
            profile->files = gbl.files;
        }

        prof_count++;
    }

    toml_datum_t pkg_tab = toml_get(top, "package");
    toml_datum_t items = toml_get(pkg_tab, "items");
    size_t pkg_num = (size_t)items.u.arr.size;
    Package* pkgs = malloc(sizeof(Package) * pkg_num);
    for (size_t i = 0; i < pkg_num; i++) {
        toml_datum_t profile_table = items.u.arr.elem[i];
        pkgs[i].name = toml_get(profile_table, "name").u.s;
        pkgs[i].version = toml_get(profile_table, "version").u.s;
    }

    State state = {.global = gbl,
                   .profiles_num = prof_count,
                   .profiles = profs,
                   .package_num = pkg_num,
                   .packages = pkgs,
                   .toml_res = res};

    return state;
}
