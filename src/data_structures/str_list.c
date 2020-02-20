#include "str_list.h"

struct str_list *strl_allocate() {
    struct str_list *strl = (struct str_list *) malloc(sizeof(struct str_list));
    strl->list = (char **)malloc( INITIAL_STRL_LENGTH * sizeof(char *) );
    strl->filled = 0;
    strl->capacity = INITIAL_STRL_LENGTH;
    return strl;
}

void strl_deallocate(struct str_list *strl) {
    int idx;
    for(idx = 0; idx < strl->filled; ++idx) {
        free(strl->list[idx]);
    }
    free(strl->list);
    free(strl);
    return;
}

void strl_append(struct str_list *strl, const char *str) {
    ssize_t capacity = strl->capacity;

    if (strl->filled == capacity) {
        capacity = capacity * 2 + 1;
        strl->list = (char **) realloc(strl->list, capacity * sizeof(char *));
        if (strl->list == NULL) {
            die("Could not grow strl!");
        }
        strl->capacity = capacity;
    }

    strl->list[strl->filled] = (char *) malloc(strlen(str) + 1);
    strcpy(strl->list[strl->filled], str);
    strl->filled += 1;
    return;
}

ssize_t strl_len(struct str_list *strl) {
    return strl->filled;
}

char *strl_get(struct str_list *strl, int idx) {
    if (idx > strl->filled) {
        return NULL;
    }
    return strl->list[idx];
}
