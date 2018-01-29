# C++ wrapper library for c_Plus
cmake_minimum_required (VERSION 3.1)
project (c_Plus_C++Lib VERSION 1.0.0 LANGUAGES CXX)

set (SOURCES
    src/array.cc
)

set (HEADERS
    include/c_plus/array
)

add_library (cpluslib SHARED ${SOURCES} ${HEADERS})

target_include_directories (cpluslib PUBLIC include)
target_link_libraries (cpluslib PRIVATE cplus)
set_property (TARGET cpluslib PROPERTY CXX_STANDARD 11)

#----------------------------------------------------------------------------------------

add_executable (c+lib ../../tests/c++.cc)
set_property (TARGET c+lib PROPERTY CXX_STANDARD 11)
target_link_libraries (c+lib PRIVATE cpluslib)
