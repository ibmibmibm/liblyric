set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
include_directories(../libjson)
add_library(lyric lyric.c singer.c tag.c line.c parser.c)
