set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
add_library(lyric lyric.c singer.c tag.c line.c json-object.c json-parser.c json-util.c json-arraylist.c json-linkhash.c parser.c)
