#include <stdio.h>
#include <errno.h>
#include <lyric.h>
#include <property.h>

int usage(const char *program_name) {
    if (errno != 0)
        perror(program_name);
    fprintf(stderr, "usage; %s filename.\n", program_name);
    return 0;
}

int main(int argc, const char *argv[]) {
    FILE *lyric_file;
    Lyric *lyric;

    if (argc != 2)
        return usage(argv[0]);

    lyric_file = fopen(argv[1], "r");
    if (lyric_file == NULL)
        return usage(argv[0]);

    lyric = lyric_lyric_new_from_file(lyric_file);
    lyric_property_push_back(lyric->property, "lyricist", "test");
    lyric_lyric_save_to_file(lyric, stdout);
    lyric_lyric_delete(lyric);
    fclose(lyric_file);
    return 0;
}
