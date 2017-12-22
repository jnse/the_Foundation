cmake_minimum_required (VERSION 3.1)
project (c_Plus VERSION 1.0.0 LANGUAGES C)
include (CheckIncludeFile)
include (TestBigEndian)
find_package (PkgConfig)

macro (cplus_set_compile_options target)
    set_property (TARGET ${target} PROPERTY C_STANDARD 11)
    target_compile_options (${target} PRIVATE -Wall -Wpedantic -Werror)
    if (CMAKE_COMPILER_IS_GNUCC)
        target_compile_options (${target} PRIVATE -Wno-format)
        if (iHaveSSE4_1)
            target_compile_options (${target} PUBLIC -msse4.1)
        endif ()
    else ()
        target_compile_options (${target} PRIVATE -Wno-format-pedantic)
    endif ()
endmacro ()

#----------------------------------------------------------------------------------------

option (CPLUS_ENABLE_SSE41 "Enable SSE 4.1 instructions" ON)

# Regular expressions
pkg_check_modules (PCRE libpcre)
if (PCRE_FOUND)
    set (iHavePcre YES)
endif ()

# zlib
find_package (ZLIB)
if (ZLIB_FOUND)
    set (iHaveZlib YES)
endif ()

# C11 threads
check_include_file (threads.h iHaveC11Threads)

# SSE 4.1 instruction set
if (CPLUS_ENABLE_SSE41)
    check_include_file (smmintrin.h iHaveSSE4_1)
else ()
    set (iHaveSSE4_1 NO)
endif ()
if (iHaveSSE4_1)
    set (mathSpec sse)
else ()
    set (mathSpec generic)
endif ()

test_big_endian (iHaveBigEndian)

#----------------------------------------------------------------------------------------

set (HEADERS
    include/c11threads.h
    include/stdthreads.h
    include/c_plus/defs.h
    include/c_plus/audience.h
    include/c_plus/array.h
    include/c_plus/block.h
    include/c_plus/blockhash.h
    include/c_plus/buffer.h
    include/c_plus/class.h
    include/c_plus/commandline.h
    include/c_plus/file.h
    include/c_plus/fileinfo.h
    include/c_plus/future.h
    include/c_plus/garbage.h
    include/c_plus/hash.h
    include/c_plus/list.h
    include/c_plus/map.h
    include/c_plus/math.h
    include/c_plus/math_${mathSpec}.h
    include/c_plus/mutex.h
    include/c_plus/object.h
    include/c_plus/objectlist.h
    include/c_plus/path.h
    include/c_plus/ptrarray.h
    include/c_plus/ptrset.h
    include/c_plus/queue.h
    include/c_plus/range.h
    include/c_plus/socket.h
    include/c_plus/sortedarray.h
    include/c_plus/stream.h
    include/c_plus/string.h
    include/c_plus/stringarray.h
    include/c_plus/stringlist.h
    include/c_plus/stringhash.h
    include/c_plus/thread.h
    include/c_plus/threadpool.h
    include/c_plus/treenode.h
    include/c_plus/time.h
    include/c_plus/version.h
)
set (SOURCES
    src/c_plus.c
    src/audience.c
    src/array.c
    src/block.c
    src/blockhash.c
    src/buffer.c
    src/class.c
    src/commandline.c
    src/crc32.c
    src/file.c
    src/fileinfo.c
    src/future.c
    src/garbage.c
    src/hash.c
    src/list.c
    src/map.c
    src/mutex.c
    src/math.c
    src/math_${mathSpec}.c
    src/object.c
    src/objectlist.c
    src/path.c
    src/ptrarray.c
    src/ptrset.c
    src/queue.c
    src/sortedarray.c
    src/stream.c
    src/string.c
    src/stringarray.c
    src/stringhash.c
    src/stringlist.c
    src/thread.c
    src/threadpool.c
    src/time.c
    src/treenode.c
    src/version.c
)
if (APPLE)
    set (iPlatformApple YES)
    set (SOURCES ${SOURCES} src/platform_apple.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set (iPlatformLinux YES)
    set (SOURCES ${SOURCES} src/platform_linux.c)
elseif (WIN32)
    set (iPlatformWindows YES)
    set (SOURCES ${SOURCES} src/platform_windows.c)
else ()
    set (iPlatformOther YES)
    set (SOURCES ${SOURCES} src/platform_generic.c)
endif ()
if (PCRE_FOUND)
    set (SOURCES ${SOURCES} src/regexp.c)
    set (HEADERS ${HEADERS} include/c_plus/regexp.h)
endif ()
configure_file (config.h.in config.h)

add_library (cplus STATIC ${SOURCES} ${HEADERS})

target_compile_definitions (cplus PRIVATE iIsLibraryBuild)
target_include_directories (cplus PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})
cplus_set_compile_options (cplus)

# Dependencies.
target_link_libraries (cplus PRIVATE m pthread)
if (ZLIB_FOUND)
    target_include_directories (cplus PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries      (cplus PRIVATE ${ZLIB_LIBRARIES})
endif ()
if (PCRE_FOUND)
    target_include_directories (cplus PRIVATE ${PCRE_INCLUDE_DIRS})
    target_link_libraries      (cplus PRIVATE ${PCRE_LDFLAGS})
endif ()

#----------------------------------------------------------------------------------------

macro (cplus_add_test target sources)
    add_executable (${target} ${sources})
    target_link_libraries (${target} PRIVATE cplus)
    cplus_set_compile_options (${target})
endmacro ()

cplus_add_test (c+test      tests/main.c)
cplus_add_test (c+strings   tests/strings.c)
cplus_add_test (c+threading tests/threading.c)
cplus_add_test (c+math      tests/mathops.c)
cplus_add_test (c+network   tests/network.c)
