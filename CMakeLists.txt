cmake_minimum_required (VERSION 3.1)
project (c_Plus VERSION 1.0.0 LANGUAGES C)
find_package (PkgConfig)

#----------------------------------------------------------------------------------------

pkg_check_modules (PCRE libpcre)
find_package (ZLIB)
if (PCRE_FOUND)
    set (iHavePcre YES)
endif ()
if (ZLIB_FOUND)
    set (iHaveZlib YES)
endif ()
configure_file (config.h.in config.h)

#----------------------------------------------------------------------------------------

set (HEADERS
    include/c_plus/defs.h
    include/c_plus/array.h
    include/c_plus/block.h
    include/c_plus/blockhash.h
    include/c_plus/class.h
    include/c_plus/file.h
    include/c_plus/fileinfo.h
    include/c_plus/garbage.h
    include/c_plus/hash.h
    include/c_plus/list.h
    include/c_plus/map.h
    include/c_plus/math.h
    include/c_plus/object.h
    include/c_plus/objectlist.h
    include/c_plus/path.h
    include/c_plus/ptrarray.h
    include/c_plus/ptrset.h
    include/c_plus/range.h
    include/c_plus/sortedarray.h
    include/c_plus/stream.h
    include/c_plus/string.h
    include/c_plus/stringarray.h
    include/c_plus/stringlist.h
    include/c_plus/stringhash.h
    include/c_plus/treenode.h
    include/c_plus/time.h
    include/c_plus/version.h
)
set (SOURCES
    src/c_plus.c
    src/array.c
    src/block.c
    src/blockhash.c
    src/class.c
    src/crc32.c
    src/file.c
    src/fileinfo.c
    src/garbage.c
    src/hash.c
    src/list.c
    src/map.c
    src/math.c
    src/object.c
    src/objectlist.c
    src/ptrarray.c
    src/ptrset.c
    src/sortedarray.c
    src/stream.c
    src/string.c
    src/stringarray.c
    src/stringhash.c
    src/stringlist.c
    src/time.c
    src/treenode.c
    src/version.c
)
if (PCRE_FOUND)
    set (SOURCES ${SOURCES} src/regexp.c)
    set (HEADERS ${HEADERS} include/c_plus/regexp.h)
endif ()
add_library (cplus STATIC ${SOURCES} ${HEADERS})
target_compile_definitions (cplus PRIVATE
    iIsLibraryBuild
)
set_property (TARGET cplus PROPERTY C_STANDARD 11)
target_include_directories (cplus PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options (cplus PRIVATE -Wall -Wpedantic -Werror)
if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options (cplus PRIVATE -Wno-format)
else ()
    target_compile_options (cplus PRIVATE -Wno-format-pedantic)
endif ()
if (ZLIB_FOUND)
    target_include_directories (cplus PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries (cplus PRIVATE ${ZLIB_LIBRARIES})
endif ()
if (PCRE_FOUND)
    target_include_directories (cplus PRIVATE ${PCRE_INCLUDE_DIRS})
    target_link_libraries (cplus PRIVATE ${PCRE_LDFLAGS})
endif ()

#----------------------------------------------------------------------------------------

add_executable (c+test tests/main.c)
target_link_libraries (c+test PRIVATE cplus)
