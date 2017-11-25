cmake_minimum_required (VERSION 3.1)
project (CPLUS
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
    include/lite/defs.h
    include/lite/array.h
    include/lite/block.h
    include/lite/class.h
    include/lite/counted.h
    include/lite/garbage.h
    include/lite/hash.h
    include/lite/list.h
    include/lite/object.h
    include/lite/ptrarray.h
    include/lite/ptrset.h
    include/lite/range.h
    include/lite/set.h
    include/lite/string.h
)
if (PCRE_FOUND)
    set (SOURCES ${SOURCES} src/regexp.c)
    set (HEADERS ${HEADERS} include/lite/regexp.h)
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
