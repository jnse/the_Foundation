cmake_minimum_required (VERSION 3.1)
project (CPLUS
    VERSION 1.0.0
    LANGUAGES C
)
set (SOURCES 
    src/array.c
    src/main.c
    src/object.c
)
set (HEADERS
    include/lite/array.h
    include/lite/defs.h
    include/lite/object.h
    include/lite/range.h
)
add_executable (cplus ${SOURCES} ${HEADERS})
set_property (TARGET cplus PROPERTY C_STANDARD 11)
target_include_directories (cplus PUBLIC include)
