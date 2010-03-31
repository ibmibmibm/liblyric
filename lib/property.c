#include <stdlib.h>
#include <string.h>
#include "property.h"
#include "helpers.h"

static bool _extend_array(Property *property);

Property* lyric_property_new(void) {
    Property *property = (Property*)calloc(sizeof(Property), 1);
    if (property == NULL) {
        return NULL;
    }
    property->size = 0;
    property->_malloc_size = 1;
    property->key = (char**)malloc(sizeof(char*));
    if (property->key == NULL) {
        lyric_property_delete(property);
        return NULL;
    }
    property->value = (char**)malloc(sizeof(char*));
    if (property->value == NULL) {
        lyric_property_delete(property);
        return NULL;
    }
    return property;
}

Property* lyric_property_new_copy(const Property *_property) {
    size_t i;
    Property *property = (Property*)calloc(sizeof(Property), 1);
    if (property == NULL) {
        return NULL;
    }
    property->size = _property->size;
    property->_malloc_size = _property->_malloc_size;
    property->key = (char**)calloc(sizeof(char*), property->_malloc_size);
    if (property->key == NULL) {
        lyric_property_delete(property);
        return NULL;
    }
    property->value = (char**)calloc(sizeof(char*), property->_malloc_size);
    if (property->value == NULL) {
        lyric_property_delete(property);
        return NULL;
    }
    for (i = 0; i < property->size; ++i) {
        property->key[i] = (char*)malloc(strlen(_property->key[i]) + 1);
        if (property->key[i] == NULL) {
            lyric_property_delete(property);
            return NULL;
        }
        property->value[i] = (char*)malloc(strlen(_property->value[i]) + 1);
        if (property->value[i] == NULL) {
            lyric_property_delete(property);
            return NULL;
        }
        strcpy(property->key[i], _property->key[i]);
        strcpy(property->value[i], _property->value[i]);
    }
    return property;
}

Property* lyric_property_new_from_file(FILE *file) {
    char key[128], value[128];
    Property *property = (Property*)calloc(sizeof(Property), 1);
    if (property == NULL) {
        return NULL;
    }
    if (!_match_token(file, "tag"))
        return NULL;
    if (!_match_token(file, "{"))
        return NULL;
    property->size = 0;
    property->_malloc_size = 1;
    property->key = (char**)malloc(sizeof(char*));
    if (property->key == NULL) {
        lyric_property_delete(property);
        return NULL;
    }
    property->value = (char**)malloc(sizeof(char*));
    if (property->value == NULL) {
        lyric_property_delete(property);
        return NULL;
    }
    while (fscanf(file, " %128[^=}\n]=%128[^}\n]", key, value) == 2) {
        lyric_property_push_back(property, key, value);
    }
    if (!_match_token(file, "}")) {
        lyric_property_delete(property);
        return NULL;
    }
    return property;
}

void lyric_property_save_to_file(const Property *property, FILE *file) {
    size_t i;
    if (property == NULL)
        return;
    fputs("tag {\n", file);
    for (i = 0; i < property->size; ++i) {
        fprintf(file, "%s=%s\n", property->key[i], property->value[i]);
    }
    fputs("}\n", file);
}

void lyric_property_delete(Property *property) {
    size_t i;
    if (property == NULL)
        return;
    if (property->key) {
        for (i = 0; i < property->size; ++i)
            free(property->key[i]);
        free(property->key);
    }
    if (property->value) {
        for (i = 0; i < property->size; ++i)
            free(property->value[i]);
        free(property->value);
    }
    free(property);
}

bool lyric_property_push_back(Property *property, const char *key, const char *value) {
    if (property == NULL)
        return false;
    return lyric_property_insert(property, property->size, key, value);
}

bool lyric_property_insert(Property *property, const size_t position, const char *key, const char *value) {
    size_t i;
    char *new_key;
    char *new_value;
    if (property == NULL)
        return false;
    if (property->size < position)
        return false;
    if (property->size > property->_malloc_size)
        if (!_extend_array(property))
            return false;
    new_key = (char*)malloc(strlen(key) + 1);
    if (new_key == NULL)
        return false;
    new_value = (char*)malloc(strlen(value) + 1);
    if (new_value == NULL) {
        free(new_key);
        return false;
    }
    strcpy(new_key, key);
    strcpy(new_value, value);
    for (i = property->size; i > position; --i) {
        char *temp;
        temp = property->key[i];
        property->key[i] = property->key[i - 1];
        property->key[i - 1] = temp;
        temp = property->value[i];
        property->value[i] = property->value[i - 1];
        property->value[i - 1] = temp;
    }
    property->key[i] = new_key;
    property->value[i] = new_value;
    ++property->size;
    return true;
}

void lyric_property_pop_back(Property *property, char **key, char **value) {
    if (property == NULL)
        return;
    return lyric_property_remove(property, property->size - 1, key, value);
}

void lyric_property_remove(Property *property, const size_t position, char **key, char **value) {
    size_t i;

    if (property->size <= position)
        return;

    if (key == NULL)
        free(property->key[position]);
    else
        *key = property->key[position];

    if (value == NULL)
        free(property->value[position]);
    else
        *value = property->value[position];

    for (i = position + 1; i < property->size; ++i) {
        property->key[i - 1] = property->key[i];
        property->value[i - 1] = property->value[i];
    }
    --property->size;
}

static bool _extend_array(Property *property) {
    size_t new_malloc_size = property->_malloc_size * 2;
    char **new_key_array;
    char **new_value_array;
    new_key_array = (char**) realloc(property->key, sizeof(char*) * new_malloc_size);
    if (new_key_array == NULL)
        return false;
    new_value_array = (char**) realloc(property->value, sizeof(char*) * new_malloc_size);
    if (new_value_array == NULL) {
        free(new_key_array);
        return false;
    }
    property->key = new_key_array;
    property->value = new_value_array;
    property->_malloc_size = new_malloc_size;
    return true;
}
