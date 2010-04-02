#ifndef __JSON_PRIVATE_H__
#define __JSON_PRIVATE_H__
#include "json.h"
#include "json-arraylist.h"
#include "json-linkhash.h"

#define hexdigit(x) (((x) <= '9') ? (x) - '0' : ((x) & 7) + 9)

typedef void (json_object_delete_fn)(struct json_object *o);
struct json_object {
    size_t _ref_count;
    json_object_delete_fn *_delete;
    enum json_type o_type;
    union data {
        bool c_boolean;
        double c_double;
        int c_int;
        struct lh_table *c_object;
        struct array_list *c_array;
        char *c_string;
    } o;
};
#endif // __JSON_PRIVATE_H__
