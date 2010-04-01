#include <stdio.h>
#include <errno.h>
#include <lyric.h>

int usage(const char *program_name) {
    if (errno != 0)
        perror(program_name);
    fprintf(stderr, "usage; %s filename.\n", program_name);
    return 0;
}

int main(int argc, const char *argv[]) {
    if (argc != 2)
        return usage(argv[0]);

    FILE *file = fopen(argv[1], "r");
    Lyric *lyric = lyric_read_file(file);
    if (lyric == NULL)
        return usage(argv[0]);
    lyric_write_file(lyric, stdout);
    lyric_lyric_delete(lyric);
    return 0;
}
