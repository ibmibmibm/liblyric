#include "json.h"
#include "json-private.h"

struct json_object* json_object_from_file(FILE *file) {
    char *buffer;
    struct json_object *obj;
    if (file == NULL) {
        goto err0;
    }
    fseek(file, 0, SEEK_END);
    const size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = malloc(file_size);
    if (buffer == NULL) {
        goto err1;
    }
    const size_t count = fread(buffer, 1, file_size, file);
    if (count != file_size) {
        goto err2;
    }
    fclose(file);
    obj = json_tokener_parse(buffer);
    free(buffer);
    return obj;
err2:
    free(buffer);
err1:
    fclose(file);
err0:
    return NULL;
}
