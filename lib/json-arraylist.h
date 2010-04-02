#ifndef __JSON_ARRAYLIST_H__
#define __JSON_ARRAYLIST_H__

#include "json.h"

#define ARRAY_LIST_DEFAULT_SIZE 32

typedef void (array_list_free_fn) (void *data);

struct array_list {
    void **array;
    size_t length;
    size_t size;
    array_list_free_fn *free_fn;
};

extern struct array_list* array_list_new(array_list_free_fn *free_fn);

extern void array_list_free(struct array_list *al);

extern void* array_list_get_idx(struct array_list *al, size_t i);

extern bool array_list_put_idx(struct array_list *al, size_t i, void *data);

extern bool array_list_add(struct array_list *al, void *data);

extern size_t array_list_length(struct array_list *al);

#endif // __JSON_ARRAYLIST_H__
