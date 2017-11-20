cmake_minimum_required (VERSION 3.1)
project (CPLUS
    VERSION 1.0.0
    LANGUAGES C
)
set (SOURCES 
    src/main.c
    src/object.c
)
set (HEADERS
    src/object.h
)
add_executable (cplus ${SOURCES} ${HEADERS})
