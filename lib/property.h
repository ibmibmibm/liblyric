#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <stdio.h>
#include <stdbool.h>

struct _Property {
    size_t size;
    char **key;
    char **value;
    size_t _malloc_size;
};
typedef struct _Property Property;

Property* lyric_property_new(void);
Property* lyric_property_new_copy(const Property *_property);
Property* lyric_property_new_from_file(FILE *file);
void lyric_property_save_to_file(const Property *property, FILE *file);
void lyric_property_delete(Property *property);
bool lyric_property_insert(Property *property, const size_t position, const char *key, const char *value);
void lyric_property_remove(Property *property, const size_t position, char **key, char **value);
bool lyric_property_push_back(Property *property, const char *key, const char *value);
void lyric_property_pop_back(Property *property, char **key, char **value);

#endif // __PROPERTY_H__
