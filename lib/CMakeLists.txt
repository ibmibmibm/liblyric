set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -fstrict-aliasing -Wall -Wextra -Wstrict-aliasing -Wno-declaration-after-statement -Wno-unused-parameter")
include_directories(../libjson)
add_library(lyric
    lyric.c singer.c tags.c line.c word.c time.c buffer.c
    parser.c lrc_parser.c
    ../libjson/json.c)
