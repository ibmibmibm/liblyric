#include "json-arraylist.h"

struct array_list* array_list_new(array_list_free_fn *free_fn) {
    struct array_list *arr;

    arr = (struct array_list*) calloc(1, sizeof(struct array_list));
    if (arr == NULL)
        return NULL;
    arr->size = ARRAY_LIST_DEFAULT_SIZE;
    arr->length = 0;
    arr->free_fn = free_fn;
    arr->array = (void**)calloc(sizeof(void*), arr->size);
    if (arr->array == NULL) {
        free(arr);
        return NULL;
    }
    return arr;
}

extern void array_list_free(struct array_list *arr) {
    for (size_t i = 0; i < arr->length; i++)
        if (arr->array[i]) arr->free_fn(arr->array[i]);
    free(arr->array);
    free(arr);
}

void* array_list_get_idx(struct array_list *arr, size_t i) {
    if (i >= arr->length) return NULL;
    return arr->array[i];
}

static bool array_list_expand_internal(struct array_list *arr, size_t max) {
    void *t;
    size_t new_size;

    if (max < arr->size)
        return true;
    new_size = json_max(arr->size << 1, max);
    t = realloc(arr->array, new_size*sizeof(void*));
    if (t == NULL)
        return false;
    arr->array = (void**)t;
    (void)memset(arr->array + arr->size, 0, (new_size-arr->size)*sizeof(void*));
    arr->size = new_size;
    return true;
}

bool array_list_put_idx(struct array_list *arr, size_t idx, void *data) {
    if (array_list_expand_internal(arr, idx))
        return false;
    if (arr->array[idx])
        arr->free_fn(arr->array[idx]);
    arr->array[idx] = data;
    if (arr->length <= idx)
        arr->length = idx + 1;
    return true;
}

bool array_list_add(struct array_list *arr, void *data) {
    return array_list_put_idx(arr, arr->length, data);
}

size_t array_list_length(struct array_list *arr) {
    return arr->length;
}

