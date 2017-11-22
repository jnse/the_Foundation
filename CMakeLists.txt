cmake_minimum_required (VERSION 3.1)
project (CPLUS
    VERSION 1.0.0
    LANGUAGES C
)
set (SOURCES
    src/array.c
    src/class.c
    src/counted.c
    src/main.c
    src/object.c
    src/ptrarray.c
    src/ptrset.c
    src/set.c
)
set (HEADERS
    include/lite/array.h
    include/lite/class.h
    include/lite/counted.h
    include/lite/defs.h
    include/lite/object.h
    include/lite/ptrarray.h
    include/lite/ptrset.h
    include/lite/range.h
    include/lite/set.h
)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic -Wno-format-pedantic")
add_executable (cplus ${SOURCES} ${HEADERS})
set_property (TARGET cplus PROPERTY C_STANDARD 11)
target_include_directories (cplus PUBLIC include)
