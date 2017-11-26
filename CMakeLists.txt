cmake_minimum_required (VERSION 3.1)
project (c_Plus
    VERSION 1.0.0
    LANGUAGES C
)

find_package (PkgConfig)

#----------------------------------------------------------------------------------------

pkg_check_modules (PCRE libpcre)
find_package (ZLIB)
if (PCRE_FOUND)
    add_definitions (-DiHavePcre)
endif ()
if (ZLIB_FOUND)
    add_definitions (-DiHaveZlib)
endif ()

#----------------------------------------------------------------------------------------

set (SOURCES
    src/array.c
    src/block.c
    src/class.c
    src/counted.c
    src/garbage.c
    src/hash.c
    src/list.c
    src/main.c
    src/object.c
    src/ptrarray.c
    src/ptrset.c
    src/set.c
    src/string.c
)
set (HEADERS
    include/c_plus/defs.h
    include/c_plus/array.h
    include/c_plus/block.h
    include/c_plus/class.h
    include/c_plus/counted.h
    include/c_plus/garbage.h
    include/c_plus/hash.h
    include/c_plus/list.h
    include/c_plus/object.h
    include/c_plus/ptrarray.h
    include/c_plus/ptrset.h
    include/c_plus/range.h
    include/c_plus/set.h
    include/c_plus/string.h
)
if (PCRE_FOUND)
    set (SOURCES ${SOURCES} src/regexp.c)
    set (HEADERS ${HEADERS} include/c_plus/regexp.h)
endif ()

add_executable (cplus ${SOURCES} ${HEADERS})

set_property (TARGET cplus PROPERTY C_STANDARD 11)
target_include_directories (cplus PUBLIC include)
target_compile_options (cplus PRIVATE -Wall -Wpedantic -Wno-format-pedantic)

if (ZLIB_FOUND)
    target_include_directories (cplus PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries (cplus PRIVATE ${ZLIB_LIBRARIES})
endif ()
if (PCRE_FOUND)
    target_include_directories (cplus PRIVATE ${PCRE_INCLUDE_DIRS})
    target_link_libraries (cplus PRIVATE ${PCRE_LDFLAGS})
endif ()
