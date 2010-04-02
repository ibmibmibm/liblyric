#include "json.h"
#include "json-private.h"

struct json_tokener_env {
    const char *const string;
    const size_t len;
    const char *cursor;
    size_t char_offset;
    char *buffer;
    size_t buffer_size;
    size_t allocated_buffer_size;
    size_t st_pos;
    uint32_t ucs_char;
    char quote_char;
    char c;
    bool is_double;
};

static const char json_null_str[] = "null";
static const char json_true_str[] = "true";
static const char json_false_str[] = "false";

const char* const json_tokener_errors[] = {
    "success",
    "continue",
    "nesting to deep",
    "unexpected end of data",
    "unexpected character",
    "null expected",
    "boolean expected",
    "number expected",
    "array value separator ',' expected",
    "quoted object property name expected",
    "object property name separator ':' expected",
    "object value separator ',' expected",
    "invalid string sequence",
    "expected comment",
};

struct json_tokener* json_tokener_new(void) {
    struct json_tokener *tok = (struct json_tokener*)calloc(1, sizeof(struct json_tokener));
    if (tok == NULL)
        return NULL;
    json_tokener_reset(tok);
    return tok;
}

void json_tokener_free(struct json_tokener *tok) {
    json_tokener_reset(tok);
    free(tok);
}

static void json_tokener_reset_level(struct json_tokener *tok, int depth) {
    tok->stack[depth].state = json_tokener_state_eatws;
    tok->stack[depth].saved_state = json_tokener_state_start;
    json_object_put(tok->stack[depth].current);
    tok->stack[depth].current = NULL;
    free(tok->stack[depth].obj_field_name);
    tok->stack[depth].obj_field_name = NULL;
}

void json_tokener_reset(struct json_tokener *tok) {
    if (tok == NULL)
        return;
    for(int i = tok->depth; i >= 0; i--)
        json_tokener_reset_level(tok, i);
    tok->depth = 0;
    tok->err = json_tokener_success;
}

struct json_object* json_tokener_parse(const char *str) {
    struct json_tokener* tok;
    struct json_object* obj;

    tok = json_tokener_new();
    obj = json_tokener_parse_ex(tok, str, -1);
    printf("%p\n", obj);

    if (tok->err != json_tokener_success) {
        obj = (struct json_object*) -tok->err;
    }
    json_tokener_free(tok);
    return obj;
}

static void reset_buffer(struct json_tokener_env *env) {
    env->buffer_size = 0;
}

static void write_char(struct json_tokener_env *env, char c) {
    if (++env->buffer_size == env->allocated_buffer_size) {
        env->allocated_buffer_size += JSON_PAGE_SIZE;
        // XXX: error handle
        env->buffer = realloc(env->buffer, env->allocated_buffer_size);
    }
    env->buffer[env->buffer_size] = c;
}

static void write_string(struct json_tokener_env *env, const char *const str, size_t size) {
    if (env->buffer_size + size >= env->allocated_buffer_size) {
        while (env->buffer_size + size >= env->allocated_buffer_size) {
            env->allocated_buffer_size += JSON_PAGE_SIZE;
        }
        // XXX: error handle
        env->buffer = realloc(env->buffer, env->allocated_buffer_size);
    }
    strncpy(&env->buffer[env->buffer_size], str, size);
    env->buffer_size += size;
}

static bool pop_char(struct json_tokener *tok, struct json_tokener_env *env) {
    if (env->char_offset == env->len) {
        if (tok->depth == 0 && tok->stack[tok->depth].state == json_tokener_state_eatws && tok->stack[tok->depth].saved_state == json_tokener_state_finish) {
            tok->err = json_tokener_success;
            return false;
        } else {
            tok->err = json_tokener_continue;
            return false;
        }
    } else {
        env->c = *env->cursor;
        return true;
    }
}
static bool advance_char(struct json_tokener *tok, struct json_tokener_env *env) {
    ++env->cursor;
    ++env->char_offset;
    return env->c != '\0';
}

static void json_tokener_switch(struct json_tokener *tok, struct json_tokener_env *env);

static void json_tokener_eatws(struct json_tokener *tok, struct json_tokener_env *env) {
    while (isspace(env->c)) {
        if (!advance_char(tok, env) || !pop_char(tok, env))
            return;
    }
    if (env->c == '/') {
        reset_buffer(env);
        write_char(env, env->c);
        tok->stack[tok->depth].state = json_tokener_state_comment_start;
        if (!advance_char(tok, env) || !pop_char(tok, env))
            return;
    } else {
        tok->stack[tok->depth].state = tok->stack[tok->depth].saved_state;
    }
    return json_tokener_switch(tok, env);
}

static void json_tokener_start(struct json_tokener *tok, struct json_tokener_env *env) {
    switch (env->c) {
        case '{':
            tok->stack[tok->depth].state = json_tokener_state_eatws;
            tok->stack[tok->depth].saved_state = json_tokener_state_object_field_start;
            tok->stack[tok->depth].current = json_object_new_object();
            break;
        case '[':
            tok->stack[tok->depth].state = json_tokener_state_eatws;
            tok->stack[tok->depth].saved_state = json_tokener_state_array;
            tok->stack[tok->depth].current = json_object_new_array();
            break;
        case 'N':
        case 'n':
            tok->stack[tok->depth].state = json_tokener_state_null;
            reset_buffer(env);
            env->st_pos = 0;
            return json_tokener_switch(tok, env);
        case '"':
        case '\'':
            tok->stack[tok->depth].state = json_tokener_state_string;
            reset_buffer(env);
            env->quote_char = env->c;
            break;
        case 'T':
        case 't':
        case 'F':
        case 'f':
            tok->stack[tok->depth].state = json_tokener_state_boolean;
            reset_buffer(env);
            env->st_pos = 0;
            return json_tokener_switch(tok, env);
#if defined(__GNUC__)
        case '0' ... '9':
#else
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
#endif
        case '-':
            tok->stack[tok->depth].state = json_tokener_state_number;
            reset_buffer(env);
            env->is_double = false;
            return json_tokener_switch(tok, env);
        default:
            tok->err = json_tokener_error_parse_unexpected;
            return;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_finish(struct json_tokener *tok, struct json_tokener_env *env) {
    if (tok->depth == 0)
        return;
    tok->obj = json_object_get(tok->stack[tok->depth].current);
    json_tokener_reset_level(tok, tok->depth);
    --tok->depth;
    return json_tokener_switch(tok, env);
}

static void json_tokener_null(struct json_tokener *tok, struct json_tokener_env *env) {
    write_char(env, env->c);
    if (strncasecmp(json_null_str, env->buffer, json_min(env->st_pos + 1, sizeof(json_null_str) - 1)) == 0) {
        if (env->st_pos == sizeof(json_null_str) - 1) {
            tok->stack[tok->depth].current = NULL;
            tok->stack[tok->depth].saved_state = json_tokener_state_finish;
            tok->stack[tok->depth].state = json_tokener_state_eatws;
            return json_tokener_switch(tok, env);
        }
    } else {
        tok->err = json_tokener_error_parse_null;
        return;
    }
    env->st_pos++;
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_comment_start(struct json_tokener *tok, struct json_tokener_env *env) {
    if (env->c == '*') {
        tok->stack[tok->depth].state = json_tokener_state_comment;
    } else if (env->c == '/') {
        tok->stack[tok->depth].state = json_tokener_state_comment_eol;
    } else {
        tok->err = json_tokener_error_parse_comment;
        return;
    }
    write_char(env, env->c);
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_comment(struct json_tokener *tok, struct json_tokener_env *env) {
    // Advance until we change state
    const char *const case_start = env->cursor;
    while (env->c != '*') {
        if (!advance_char(tok, env) || !pop_char(tok, env)) {
            write_string(env, case_start, env->cursor - case_start);
            return;
        }
    }
    write_string(env, case_start, env->cursor - case_start + 1);
    tok->stack[tok->depth].state = json_tokener_state_comment_end;
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_comment_eol(struct json_tokener *tok, struct json_tokener_env *env) {
    // Advance until we change state
    const char *const case_start = env->cursor;
    while (env->c != '\n') {
        if (!advance_char(tok, env) || !pop_char(tok, env)) {
            write_string(env, case_start, env->cursor - case_start);
            return;
        }
    }
    write_string(env, case_start, env->cursor - case_start);
    tok->stack[tok->depth].state = json_tokener_state_eatws;
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_comment_end(struct json_tokener *tok, struct json_tokener_env *env) {
    write_char(env, env->c);
    if (env->c == '/') {
        tok->stack[tok->depth].state = json_tokener_state_eatws;
    } else {
        tok->stack[tok->depth].state = json_tokener_state_comment;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_string(struct json_tokener *tok, struct json_tokener_env *env) {
    // Advance until we change state
    const char *const case_start = env->cursor;
    while (true) {
        if (env->c == env->quote_char) {
            write_string(env, case_start, env->cursor - case_start);
            tok->stack[tok->depth].current = json_object_new_string(env->buffer);
            tok->stack[tok->depth].saved_state = json_tokener_state_finish;
            tok->stack[tok->depth].state = json_tokener_state_eatws;
            break;
        } else if (env->c == '\\') {
            write_string(env, case_start, env->cursor - case_start);
            tok->stack[tok->depth].saved_state = json_tokener_state_string;
            tok->stack[tok->depth].state = json_tokener_state_string_escape;
            break;
        }
        if (!advance_char(tok, env) || !pop_char(tok, env)) {
            write_string(env, case_start, env->cursor - case_start);
            return;
        }
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_string_escape(struct json_tokener *tok, struct json_tokener_env *env) {
    switch (env->c) {
        case '"':
        case '\\':
        case '/':
            write_char(env, env->c);
            tok->stack[tok->depth].state = tok->stack[tok->depth].saved_state;
            break;
        case 'b':
        case 'n':
        case 'r':
        case 't':
            if (env->c == 'b') write_char(env, '\b');
            else if (env->c == 'n') write_char(env, '\n');
            else if (env->c == 'r') write_char(env, '\r');
            else if (env->c == 't') write_char(env, '\t');
            tok->stack[tok->depth].state = tok->stack[tok->depth].saved_state;
            break;
        case 'u':
            env->ucs_char = 0;
            env->st_pos = 0;
            tok->stack[tok->depth].state = json_tokener_state_escape_unicode;
            break;
        default:
            tok->err = json_tokener_error_parse_string;
            return;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_escape_unicode(struct json_tokener *tok, struct json_tokener_env *env) {
    /* Note that the following code is inefficient for handling large
     * chunks of extended chars, calling printbuf_memappend() once
     * for each multi-byte character of input.
     * This is a good area for future optimization.
     */
    // Advance until we change state
    while (true) {
        if (strchr(json_hex_chars, env->c)) {
            env->ucs_char += ((unsigned int)hexdigit(env->c) << ((3-env->st_pos++)*4));
            if (env->st_pos == 4) {
                unsigned char utf_out[3];
                if (env->ucs_char < 0x80) {
                    utf_out[0] = env->ucs_char;
                    write_char(env, utf_out[0]);
                } else if (env->ucs_char < 0x800) {
                    utf_out[0] = 0xc0 | (env->ucs_char >> 6);
                    utf_out[1] = 0x80 | (env->ucs_char & 0x3f);
                    write_string(env, utf_out, 2);
                } else {
                    utf_out[0] = 0xe0 | (env->ucs_char >> 12);
                    utf_out[1] = 0x80 | ((env->ucs_char >> 6) & 0x3f);
                    utf_out[2] = 0x80 | (env->ucs_char & 0x3f);
                    write_string(env, utf_out, 3);
                }
                tok->stack[tok->depth].state = tok->stack[tok->depth].saved_state;
                break;
            }
        } else {
            tok->err = json_tokener_error_parse_string;
            return;
        }
        if (!advance_char(tok, env) || !pop_char(tok, env))
            return;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_boolean(struct json_tokener *tok, struct json_tokener_env *env) {
    write_char(env, env->c);
    if (strncasecmp(json_true_str, env->buffer, json_min(env->st_pos+1, strlen(json_true_str))) == 0) {
        if (env->st_pos == strlen(json_true_str)) {
            tok->stack[tok->depth].current = json_object_new_boolean(true);
            tok->stack[tok->depth].saved_state = json_tokener_state_finish;
            tok->stack[tok->depth].state = json_tokener_state_eatws;
            return json_tokener_switch(tok, env);
        }
    } else if (strncasecmp(json_false_str, env->buffer, json_min(env->st_pos+1, strlen(json_false_str))) == 0) {
        if (env->st_pos == strlen(json_false_str)) {
            tok->stack[tok->depth].current = json_object_new_boolean(false);
            tok->stack[tok->depth].saved_state = json_tokener_state_finish;
            tok->stack[tok->depth].state = json_tokener_state_eatws;
            return json_tokener_switch(tok, env);
        }
    } else {
        tok->err = json_tokener_error_parse_boolean;
        return;
    }
    env->st_pos++;
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_number(struct json_tokener *tok, struct json_tokener_env *env) {
    // Advance until we change state
    const char *case_start = env->cursor;
    int case_len = 0;
    int numi;
    double numd;
    while (env->c && strchr(json_number_chars, env->c)) {
        ++case_len;
        if (env->c == '.' || env->c == 'e') env->is_double = true;
        if (!advance_char(tok, env) || !pop_char(tok, env)) {
            write_string(env, case_start, case_len);
            return;
        }
    }
    if (case_len > 0) {
        write_string(env, case_start, case_len);
    }
    if (!env->is_double && sscanf(env->buffer, "%d", &numi) == 1) {
        tok->stack[tok->depth].current = json_object_new_int(numi);
    } else if (env->is_double && sscanf(env->buffer, "%lf", &numd) == 1) {
        tok->stack[tok->depth].current = json_object_new_double(numd);
    } else {
        tok->err = json_tokener_error_parse_number;
        return;
    }
    tok->stack[tok->depth].saved_state = json_tokener_state_finish;
    tok->stack[tok->depth].state = json_tokener_state_eatws;
    return json_tokener_switch(tok, env);
}

static void json_tokener_array(struct json_tokener *tok, struct json_tokener_env *env) {
    if (env->c == ']') {
        tok->stack[tok->depth].saved_state = json_tokener_state_finish;
        tok->stack[tok->depth].state = json_tokener_state_eatws;
        if (!advance_char(tok, env) || !pop_char(tok, env))
            return;
        return json_tokener_switch(tok, env);
    } else {
        if (tok->depth >= JSON_TOKENER_MAX_DEPTH-1) {
            tok->err = json_tokener_error_depth;
            return;
        }
        tok->stack[tok->depth].state = json_tokener_state_array_add;
        tok->depth++;
        json_tokener_reset_level(tok, tok->depth);
        return json_tokener_switch(tok, env);
    }
}

static void json_tokener_array_add(struct json_tokener *tok, struct json_tokener_env *env) {
    json_object_array_add(tok->stack[tok->depth].current, tok->obj);
    tok->stack[tok->depth].saved_state = json_tokener_state_array_sep;
    tok->stack[tok->depth].state = json_tokener_state_eatws;
    return json_tokener_switch(tok, env);
}

static void json_tokener_array_sep(struct json_tokener *tok, struct json_tokener_env *env) {
    if (env->c == ']') {
        tok->stack[tok->depth].saved_state = json_tokener_state_finish;
        tok->stack[tok->depth].state = json_tokener_state_eatws;
    } else if (env->c == ',') {
        tok->stack[tok->depth].saved_state = json_tokener_state_array;
        tok->stack[tok->depth].state = json_tokener_state_eatws;
    } else {
        tok->err = json_tokener_error_parse_array;
        return;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_object_field_start(struct json_tokener *tok, struct json_tokener_env *env) {
    if (env->c == '}') {
        tok->stack[tok->depth].saved_state = json_tokener_state_finish;
        tok->stack[tok->depth].state = json_tokener_state_eatws;
    } else if (env->c == '"' || env->c == '\'') {
        env->quote_char = env->c;
        reset_buffer(env);
        tok->stack[tok->depth].state = json_tokener_state_object_field;
    } else {
        tok->err = json_tokener_error_parse_object_key_name;
        return;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_object_field(struct json_tokener *tok, struct json_tokener_env *env) {
    // Advance until we change state
    const char *case_start = env->cursor;
    while (true) {
        if (env->c == env->quote_char) {
            write_string(env, case_start, env->cursor - case_start);
            tok->stack[tok->depth].obj_field_name = strdup(env->buffer);
            tok->stack[tok->depth].saved_state = json_tokener_state_object_field_end;
            tok->stack[tok->depth].state = json_tokener_state_eatws;
            break;
        } else if (env->c == '\\') {
            write_string(env, case_start, env->cursor - case_start);
            tok->stack[tok->depth].saved_state = json_tokener_state_object_field;
            tok->stack[tok->depth].state = json_tokener_state_string_escape;
            break;
        }
        if (!advance_char(tok, env) || !pop_char(tok, env)) {
            write_string(env, case_start, env->cursor - case_start);
            return;
        }
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_object_field_end(struct json_tokener *tok, struct json_tokener_env *env) {
    if (env->c == ':') {
        tok->stack[tok->depth].saved_state = json_tokener_state_object_value;
        tok->stack[tok->depth].state = json_tokener_state_eatws;
    } else {
        tok->err = json_tokener_error_parse_object_key_sep;
        return;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_object_value(struct json_tokener *tok, struct json_tokener_env *env) {
    if (tok->depth >= JSON_TOKENER_MAX_DEPTH-1) {
        tok->err = json_tokener_error_depth;
        return;
    }
    tok->stack[tok->depth].state = json_tokener_state_object_value_add;
    tok->depth++;
    json_tokener_reset_level(tok, tok->depth);
    return json_tokener_switch(tok, env);
}

static void json_tokener_object_value_add(struct json_tokener *tok, struct json_tokener_env *env) {
    json_object_object_add(tok->stack[tok->depth].current, tok->stack[tok->depth].obj_field_name, tok->obj);
    free(tok->stack[tok->depth].obj_field_name);
    tok->stack[tok->depth].obj_field_name = NULL;
    tok->stack[tok->depth].saved_state = json_tokener_state_object_sep;
    tok->stack[tok->depth].state = json_tokener_state_eatws;
    return json_tokener_switch(tok, env);
}

static void json_tokener_object_sep(struct json_tokener *tok, struct json_tokener_env *env) {
    if (env->c == '}') {
        tok->stack[tok->depth].saved_state = json_tokener_state_finish;
        tok->stack[tok->depth].state = json_tokener_state_eatws;
    } else if (env->c == ',') {
        tok->stack[tok->depth].saved_state = json_tokener_state_object_field_start;
        tok->stack[tok->depth].state = json_tokener_state_eatws;
    } else {
        tok->err = json_tokener_error_parse_object_value_sep;
        return;
    }
    if (!advance_char(tok, env) || !pop_char(tok, env))
        return;
    return json_tokener_switch(tok, env);
}

static void json_tokener_switch(struct json_tokener *tok, struct json_tokener_env *env) {
    switch (tok->stack[tok->depth].state) {
        case json_tokener_state_eatws:
            return json_tokener_eatws(tok, env);
        case json_tokener_state_start:
            return json_tokener_start(tok, env);
        case json_tokener_state_finish:
            return json_tokener_finish(tok, env);
        case json_tokener_state_null:
            return json_tokener_null(tok, env);
        case json_tokener_state_comment_start:
            return json_tokener_comment_start(tok, env);
        case json_tokener_state_comment:
            return json_tokener_comment(tok, env);
        case json_tokener_state_comment_eol:
            return json_tokener_comment_eol(tok, env);
        case json_tokener_state_comment_end:
            return json_tokener_comment_end(tok, env);
        case json_tokener_state_string:
            return json_tokener_string(tok, env);
        case json_tokener_state_string_escape:
            return json_tokener_string_escape(tok, env);
        case json_tokener_state_escape_unicode:
            return json_tokener_escape_unicode(tok, env);
        case json_tokener_state_boolean:
            return json_tokener_boolean(tok, env);
        case json_tokener_state_number:
            return json_tokener_number(tok, env);
        case json_tokener_state_array:
            return json_tokener_array(tok, env);
        case json_tokener_state_array_add:
            return json_tokener_array_add(tok, env);
        case json_tokener_state_array_sep:
            return json_tokener_array_sep(tok, env);
        case json_tokener_state_object_field_start:
            return json_tokener_object_field_start(tok, env);
        case json_tokener_state_object_field:
            return json_tokener_object_field(tok, env);
        case json_tokener_state_object_field_end:
            return json_tokener_object_field_end(tok, env);
        case json_tokener_state_object_value:
            return json_tokener_object_value(tok, env);
        case json_tokener_state_object_value_add:
            return json_tokener_object_value_add(tok, env);
        case json_tokener_state_object_sep:
            return json_tokener_object_sep(tok, env);
    }
}

struct json_object* json_tokener_parse_ex(struct json_tokener *tok, const char *str, int len) {
    struct json_tokener_env env = {
        .string = str,
        .len = len,
        .cursor = str,
        .char_offset = 0,
        .buffer = malloc(JSON_PAGE_SIZE),
        .buffer_size = 0,
        .allocated_buffer_size = JSON_PAGE_SIZE,
        .st_pos = 0,
        .ucs_char = 0,
        .quote_char = 0,
        .c = '\1',
        .is_double = false,
    };
    tok->string = str;
    tok->err = json_tokener_success;
    if (pop_char(tok, &env))
        json_tokener_switch(tok, &env);
    if (env.c) { /* We hit an eof char (0) */
        if (tok->stack[tok->depth].state != json_tokener_state_finish && tok->stack[tok->depth].saved_state != json_tokener_state_finish)
            tok->err = json_tokener_error_parse_eof;
    }

    if (tok->err == json_tokener_success)
        return json_object_get(tok->stack[tok->depth].current);
    else {
        fprintf(stderr, "json_tokener_parse_ex: error %s at offset %zd\n", json_tokener_errors[tok->err], env.char_offset);
        return NULL;
    }
}

