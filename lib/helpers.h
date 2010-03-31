#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <stdio.h>
#include <ctype.h>
#include <string.h>

static bool _match_token(FILE *file, const char *string) {
    int c;
    int i;
    do {
        c = fgetc(file);
    } while (isspace(c));
    for (i = 0; string[i] != '\0'; ++i) {
        if (c != string[i])
            return false;
        c = fgetc(file);
    }
    if (isalpha(c))
        return false;
    ungetc(c, file);
    return true;
}

#endif // __HELPERS_H__
