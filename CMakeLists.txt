cmake_minimum_required(VERSION 2.6)
project(LYRIC)

set(CMAKE_BUILD_TYPE Debug)

include(CheckCCompilerFlag)

if(MSVC)
    # MSVC needs different flags at all
    check_c_compiler_flag("/W3" HAVE_W3)
    if(HAVE_W3)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
    endif(HAVE_W3)
else(MSVC)
    # Check for extra compiler flags we want to use
    check_c_compiler_flag("-Wall" HAVE_WALL)
    if(HAVE_WALL)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    endif(HAVE_WALL)

    check_c_compiler_flag("-Wno-pointer-sign" HAVE_WPOINTERSIGN)
    if(HAVE_WPOINTERSIGN)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-sign")
    endif(HAVE_WPOINTERSIGN)

    check_c_compiler_flag("-Werror-implicit-function-declaration" HAVE_WERRORIMPLICITDECLARATION)
    if(HAVE_WERRORIMPLICITDECLARATION)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration")
    endif(HAVE_WERRORIMPLICITDECLARATION)

    check_c_compiler_flag("-Wdeclaration-after-statement" HAVE_WDECLARATIONAFTERSTATEMENT)
    if(HAVE_WDECLARATIONAFTERSTATEMENT)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdeclaration-after-statement")
    endif(HAVE_WDECLARATIONAFTERSTATEMENT)

    check_c_compiler_flag("-Wpointer-arith" HAVE_WPOINTERARITH)
    if(HAVE_WPOINTERARITH)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
    endif(HAVE_WPOINTERARITH)

    check_c_compiler_flag("-Wshadow" HAVE_WSHADOW)
    if(HAVE_WSHADOW)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
    endif(HAVE_WSHADOW)

#    check_c_compiler_flag("-Werror" HAVE_WERROR)
#    if(HAVE_WERROR)
#        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
#    endif(HAVE_WERROR)

    add_definitions()
    check_c_compiler_flag("-Wp,-D_FORTIFY_SOURCE=2" HAVE_FORTIFY_SOURCE)
    if(HAVE_FORTIFY_SOURCE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wp,-D_FORTIFY_SOURCE=2")
    endif(HAVE_FORTIFY_SOURCE)
endif(MSVC)

add_subdirectory(lib)
add_subdirectory(src)
