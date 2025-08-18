#ifndef CLI_H
#define CLI_H

#include "../config/config.h"

void print_usage(void);
int print_action(const char* action_label,
                 const char* argument,
                 const char* fallback);
char* fetch_next_argument(int argc, char* argv[], int* index_ptr);
int get_opt(char option_character,
            char* option_argument,
            Config* global_config);

#endif
