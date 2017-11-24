cmake_minimum_required (VERSION 3.1)
project (CPLUS
    VERSION 1.0.0
    LANGUAGES C
)

find_package (PkgConfig)

#----------------------------------------------------------------------------------------

pkg_check_modules (PCRE libpcre)

#----------------------------------------------------------------------------------------

set (SOURCES
    src/array.c
    src/block.c
    src/class.c
    src/counted.c
    src/garbage.c
    src/list.c
    src/main.c
    src/object.c
    src/ptrarray.c
    src/ptrset.c
    src/regexp.c
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
    include/lite/list.h
    include/lite/object.h
    include/lite/ptrarray.h
    include/lite/ptrset.h
    include/lite/range.h
    include/lite/regexp.h
    include/lite/set.h
    include/lite/string.h
)
add_executable (cplus ${SOURCES} ${HEADERS})

set_property (TARGET cplus PROPERTY C_STANDARD 11)
target_include_directories (cplus PUBLIC include)
target_compile_options (cplus PUBLIC -Wall -Wpedantic -Wno-format-pedantic)

if (PCRE_FOUND)
    target_compile_definitions (cplus PUBLIC iHaveLibPcre)
    target_include_directories (cplus PUBLIC ${PCRE_INCLUDE_DIRS})
    target_link_libraries (cplus PUBLIC ${PCRE_LDFLAGS})
endif ()
