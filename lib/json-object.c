#include "json.h"
#include "json-private.h"

#define REFCOUNT_DEBUG

#ifdef REFCOUNT_DEBUG
static const char* json_type_name[] = {
    "null",
    "boolean",
    "double",
    "int",
    "object",
    "array",
    "string",
};
#endif // REFCOUNT_DEBUG

static void json_object_generic_delete(struct json_object* jso);
static struct json_object* json_object_new(enum json_type o_type);

// ref count debugging
#ifdef REFCOUNT_DEBUG
static struct lh_table *json_object_table;

static void json_object_init(void) __attribute__ ((constructor));
static void json_object_init(void) {
    fprintf(stderr, "json_object_init: creating object table\n");
    json_object_table = lh_kptr_table_new(128, "json_object_table", NULL);
}

static void json_object_fini(void) __attribute__ ((destructor));
static void json_object_fini(void) {
    struct lh_entry *ent;
    if (json_object_table->count) {
        fprintf(stderr, "json_object_fini: %d referenced objects at exit\n", json_object_table->count);
        lh_foreach(json_object_table, ent) {
            struct json_object* obj = (struct json_object*)ent->v;
            fprintf(stderr, "\t%s:%p\n", json_type_name[obj->o_type], obj);
        }
    }
    fprintf(stderr, "json_object_fini: freeing object table\n");
    lh_table_free(json_object_table);
}
#endif // REFCOUNT_DEBUG

// reference counting

extern struct json_object* json_object_get(struct json_object *jso) {
    if (jso) {
        jso->_ref_count++;
    }
    return jso;
}

extern void json_object_put(struct json_object *jso) {
    if (jso) {
        jso->_ref_count--;
        if (jso->_ref_count == 0)
            jso->_delete(jso);
    }
}

// generic object construction and destruction parts

static void json_object_generic_delete(struct json_object* jso) {
#ifdef REFCOUNT_DEBUG
    fprintf(stderr, "json_object_delete_%s: %p\n", json_type_name[jso->o_type], jso);
    lh_table_delete(json_object_table, jso);
#endif /* REFCOUNT_DEBUG */
    free(jso);
}

static struct json_object* json_object_new(enum json_type o_type) {
    struct json_object *jso;

    jso = (struct json_object*)calloc(sizeof(struct json_object), 1);
    if (!jso) return NULL;
    jso->o_type = o_type;
    jso->_ref_count = 1;
    jso->_delete = &json_object_generic_delete;
#ifdef REFCOUNT_DEBUG
    lh_table_insert(json_object_table, jso, jso);
    fprintf(stderr, "json_object_new_%s: %p\n", json_type_name[jso->o_type], jso);
#endif /* REFCOUNT_DEBUG */
    return jso;
}

// type checking functions

bool json_object_is_type(struct json_object *jso, enum json_type type) {
    return (jso->o_type == type);
}

enum json_type json_object_get_type(struct json_object *jso) {
    return jso->o_type;
}

static void json_object_lh_entry_free(struct lh_entry *ent) {
    free(ent->k);
    json_object_put((struct json_object*)ent->v);
}

static void json_object_object_delete(struct json_object* jso) {
    lh_table_free(jso->o.c_object);
    json_object_generic_delete(jso);
}

struct json_object* json_object_new_object(void) {
    struct json_object *jso = json_object_new(json_type_object);
    if (jso == NULL)
        return NULL;
    jso->_delete = &json_object_object_delete;
    jso->o.c_object = lh_kchar_table_new(JSON_OBJECT_DEF_HASH_ENTRIES, NULL, &json_object_lh_entry_free);
    return jso;
}

struct lh_table* json_object_get_object(struct json_object *jso) {
    if (jso == NULL)
        return NULL;
    switch (jso->o_type) {
        case json_type_object:
            return jso->o.c_object;
        default:
            return NULL;
    }
}

void json_object_object_add(struct json_object* jso, const char *key, struct json_object *val) {
    lh_table_delete(jso->o.c_object, key);
    lh_table_insert(jso->o.c_object, strdup(key), val);
}

struct json_object* json_object_object_get(struct json_object* jso, const char *key) {
    return (struct json_object*) lh_table_lookup(jso->o.c_object, key);
}

void json_object_object_del(struct json_object* jso, const char *key) {
    lh_table_delete(jso->o.c_object, key);
}

// json_object_boolean

struct json_object* json_object_new_boolean(bool b) {
    struct json_object *jso = json_object_new(json_type_boolean);
    if (jso == NULL)
        return NULL;
    jso->o.c_boolean = b;
    return jso;
}

bool json_object_get_boolean(struct json_object *jso) {
    if (jso == NULL)
        return false;
    switch (jso->o_type) {
        case json_type_boolean:
            return jso->o.c_boolean;
        case json_type_int:
            return (jso->o.c_int != 0);
        case json_type_double:
            return (jso->o.c_double != 0);
        case json_type_string:
            return (strlen(jso->o.c_string) != 0);
        default:
            return false;
    }
}


// json_object_int

struct json_object* json_object_new_int(int i) {
    struct json_object *jso = json_object_new(json_type_int);
    if (jso == NULL)
        return NULL;
    jso->o.c_int = i;
    return jso;
}

int json_object_get_int(struct json_object *jso) {
    if (jso == NULL)
        return 0;
    switch (jso->o_type) {
        case json_type_int:
            return jso->o.c_int;
        case json_type_double:
            return (int)jso->o.c_double;
        case json_type_boolean:
            return jso->o.c_boolean;
        case json_type_string:
            return strtol(jso->o.c_string, NULL, 10);
        default:
            return 0;
    }
}

// json_object_double

struct json_object* json_object_new_double(double d) {
    struct json_object *jso = json_object_new(json_type_double);
    if (jso == NULL)
        return NULL;
    jso->o.c_double = d;
    return jso;
}

double json_object_get_double(struct json_object *jso) {
    if (jso == NULL)
        return 0.0;
    switch (jso->o_type) {
        case json_type_double:
            return jso->o.c_double;
        case json_type_int:
            return jso->o.c_int;
        case json_type_boolean:
            return jso->o.c_boolean;
        case json_type_string:
            return strtod(jso->o.c_string, NULL);
        default:
            return 0.0;
    }
}


// json_object_string

static void json_object_string_delete(struct json_object* jso) {
    free(jso->o.c_string);
    json_object_generic_delete(jso);
}

struct json_object* json_object_new_string(const char *s) {
    struct json_object *jso = json_object_new(json_type_string);
    if (jso == NULL)
        return NULL;
    jso->_delete = &json_object_string_delete;
    jso->o.c_string = strdup(s);
    return jso;
}

struct json_object* json_object_new_string_len(const char *s, int len) {
    struct json_object *jso = json_object_new(json_type_string);
    if (jso == NULL)
        return NULL;
    jso->_delete = &json_object_string_delete;
    jso->o.c_string = strndup(s, len);
    return jso;
}

const char* json_object_get_string(struct json_object *jso) {
    if (jso == NULL)
        return NULL;
    switch (jso->o_type) {
        case json_type_string:
            return jso->o.c_string;
        default:
            return NULL;
    }
}


/* json_object_array */

static void json_object_array_entry_free(void *data) {
    json_object_put((struct json_object*)data);
}

static void json_object_array_delete(struct json_object* jso) {
    array_list_free(jso->o.c_array);
    json_object_generic_delete(jso);
}

struct json_object* json_object_new_array(void) {
    struct json_object *jso = json_object_new(json_type_array);
    if (jso == NULL)
        return NULL;
    jso->_delete = &json_object_array_delete;
    jso->o.c_array = array_list_new(&json_object_array_entry_free);
    return jso;
}

struct array_list* json_object_get_array(struct json_object *jso) {
    if (jso == NULL)
        return NULL;
    switch (jso->o_type) {
        case json_type_array:
            return jso->o.c_array;
        default:
            return NULL;
    }
}

size_t json_object_array_length(struct json_object *jso) {
    return array_list_length(jso->o.c_array);
}

bool json_object_array_add(struct json_object *jso, struct json_object *val) {
    return array_list_add(jso->o.c_array, val);
}

bool json_object_array_put_idx(struct json_object *jso, size_t idx, struct json_object *val) {
    return array_list_put_idx(jso->o.c_array, idx, val);
}

struct json_object* json_object_array_get_idx(struct json_object *jso, size_t idx) {
    return (struct json_object*)array_list_get_idx(jso->o.c_array, idx);
}

