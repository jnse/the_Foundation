cmake_minimum_required (VERSION 3.1)
project (CPLUS
    VERSION 1.0.0
    LANGUAGES C
)
set (SOURCES 
    src/list.c
    src/main.c
    src/object.c
)
set (HEADERS
    include/lite/defs.h
    include/lite/list.h
    include/lite/object.h
)
add_executable (cplus ${SOURCES} ${HEADERS})
target_include_directories (cplus PUBLIC include)
