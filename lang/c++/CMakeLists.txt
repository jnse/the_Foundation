# C++ wrapper library for the_Foundation
cmake_minimum_required (VERSION 3.1)
project (c++_Foundation VERSION 1.0.0 LANGUAGES CXX)

set (SOURCES
    src/array.cc
)

set (HEADERS
    include/t_fdn/array
)

set (TARGETNAME cpp_Foundation)
add_library (${TARGETNAME} SHARED ${SOURCES} ${HEADERS})

target_include_directories (${TARGETNAME} PUBLIC include)
target_link_libraries (${TARGETNAME} PRIVATE cplus)
set_property (TARGET ${TARGETNAME} PROPERTY CXX_STANDARD 11)

if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options (${TARGETNAME} PRIVATE -fPIC)
endif ()

#----------------------------------------------------------------------------------------

add_executable (cpptest_Foundation ../../tests/c++.cc)
set_property (TARGET cpptest_Foundation PROPERTY CXX_STANDARD 11)
target_link_libraries (cpptest_Foundation PRIVATE ${TARGETNAME})
