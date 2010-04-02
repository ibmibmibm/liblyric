#ifndef __JSON_OBJECT_H__
#define __JSON_OBJECT_H__

static const size_t JSON_OBJECT_DEF_HASH_ENTRIES = 16;
static const char json_number_chars[] = "0123456789.+-eE";
static const char json_hex_chars[] = "0123456789abcdef";

// forward structure definitions

typedef struct lh_table lh_table;
typedef struct array_list array_list;
typedef struct json_object json_object;
typedef struct json_object_iter json_object_iter;
typedef struct json_tokener json_tokener;

// supported object types
typedef enum json_type {
    json_type_null,
    json_type_boolean,
    json_type_double,
    json_type_int,
    json_type_object,
    json_type_array,
    json_type_string
} json_type;

// reference counting functions
extern struct json_object* json_object_get(struct json_object *obj);
extern void json_object_put(struct json_object *obj);
extern bool json_object_is_type(struct json_object *obj, enum json_type type);
extern enum json_type json_object_get_type(struct json_object *obj);

// object type methods
extern struct json_object* json_object_new_object(void);
extern struct lh_table* json_object_get_object(struct json_object *obj);
extern void json_object_object_add(struct json_object* obj, const char *key, struct json_object *val);
extern struct json_object* json_object_object_get(struct json_object* obj, const char *key);
extern void json_object_object_del(struct json_object* obj, const char *key);

// Array type methods
extern struct json_object* json_object_new_array(void);
extern size_t json_object_array_length(struct json_object *obj);
extern bool json_object_array_add(struct json_object *obj, struct json_object *val);
extern bool json_object_array_put_idx(struct json_object *obj, size_t idx, struct json_object *val);
extern struct json_object* json_object_array_get_idx(struct json_object *obj, size_t idx);

// boolean type methods
extern struct json_object* json_object_new_boolean(bool b);
extern bool json_object_get_boolean(struct json_object *obj);

// int type methods
extern struct json_object* json_object_new_int(int i);
extern int json_object_get_int(struct json_object *obj);

// double type methods
extern struct json_object* json_object_new_double(double d);
extern double json_object_get_double(struct json_object *obj);

// string type methods
extern struct json_object* json_object_new_string(const char *s);
extern struct json_object* json_object_new_string_len(const char *s, int len);
extern const char* json_object_get_string(struct json_object *obj);

#endif // __JSON_OBJECT_H__
