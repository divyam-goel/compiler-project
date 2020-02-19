#include "utils.h"

#ifndef STR_LIST_H

#define STR_LIST_H

#define INITIAL_STRL_LENGTH 10

struct str_list {
    char **list;
    ssize_t filled;
    ssize_t capacity;
};

struct str_list *strl_allocate();
void strl_deallocate(struct str_list *strl);
void strl_append(struct str_list *strl, const char *str);
ssize_t strl_len(struct str_list *strl);
char *strl_get(struct str_list *strl, int idx);

#endif
