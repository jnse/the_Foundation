cmake_minimum_required (VERSION 3.1)
project (the_Foundation VERSION 1.0.0 LANGUAGES C)
include (CheckIncludeFile)
include (CheckCSourceCompiles)
include (CheckFunctionExists)
include (TestBigEndian)
find_package (PkgConfig)

macro (tfdn_set_compile_options target)
    set_property (TARGET ${target} PROPERTY C_STANDARD 11)
    target_compile_options (${target} PRIVATE -Wall -Wpedantic -Werror)
    if (CMAKE_COMPILER_IS_GNUCC)
        target_compile_options (${target} PRIVATE -Wno-format)
        if (iPlatformWindows)
            target_compile_options (${target} PRIVATE
                -Wno-int-conversion
                -Wno-int-to-pointer-cast
                -Wno-pointer-to-int-cast)
        endif ()
        if (iHaveSSE4_1)
            target_compile_options (${target} PUBLIC -msse4.1)
        endif ()
    else ()
        target_compile_options (${target} PRIVATE -Wno-format-pedantic -Wno-format)
        if (iHaveSSE4_1)
            target_compile_options (${target} PUBLIC -msse4.1)
        endif ()
    endif ()
endmacro ()

#----------------------------------------------------------------------------------------

option (TFDN_ENABLE_SSE41 "Enable SSE 4.1 instructions" ON)
option (TFDN_ENABLE_DEBUG_OUTPUT "Enable internal debug output to stdout/stderr" OFF)

if (TFDN_ENABLE_DEBUG_OUTPUT)
    set (iHaveDebugOutput YES)
endif ()

# strnstr
check_function_exists (strnstr iHaveStrnstr)

# unistring
set (UNISTRING_DIR /usr CACHE PATH "Location of libunistring")
if (NOT EXISTS ${UNISTRING_DIR}/include/unistr.h)
    message (FATAL_ERROR "Not found: ${UNISTRING_DIR}/include/unistr.h (set UNISTRING_DIR)")
endif ()
if (NOT UNISTRING_DIR STREQUAL "/usr")
    set (UNISTRING_INCLUDE_DIRS ${UNISTRING_DIR}/include)
    set (UNISTRING_LIBRARIES -L${UNISTRING_DIR}/lib unistring)
else ()
    set (UNISTRING_LIBRARIES unistring)
endif ()

# zlib
pkg_check_modules (ZLIB zlib)
if (ZLIB_FOUND)
    set (iHaveZlib YES)
endif ()

# Regular expressions
pkg_check_modules (PCRE libpcre)
if (PCRE_FOUND)
    set (iHavePcre YES)
endif ()

# CURL
pkg_check_modules (CURL libcurl)
if (CURL_FOUND)
    set (iHaveCurl YES)
endif ()

# C11 threads
check_include_file (pthread.h iHavePThread)
check_include_file (threads.h iHaveC11Threads)
if (NOT iHaveC11Threads)
    if (NOT iHavePThread)
        message (FATAL_ERROR "Either pthread.h or C11 threads.h must be available")
    endif ()
    check_c_source_compiles ("
        #include <pthread.h>
        int main(int argc, char **argv) {
            return PTHREAD_MUTEX_TIMED_NP;
        }"
    iHavePThreadTimedMutex)
endif ()

# SSE 4.1 instruction set
if (TFDN_ENABLE_SSE41)
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
    include/the_Foundation/address.h
    include/the_Foundation/argcount.h
    include/the_Foundation/array.h
    include/the_Foundation/atomic.h
    include/the_Foundation/audience.h
    include/the_Foundation/block.h
    include/the_Foundation/blockhash.h
    include/the_Foundation/buffer.h
    include/the_Foundation/c11threads.h
    include/the_Foundation/class.h
    include/the_Foundation/commandline.h
    include/the_Foundation/datagram.h
    include/the_Foundation/defs.h
    include/the_Foundation/file.h
    include/the_Foundation/fileinfo.h
    include/the_Foundation/fixedpoint.h
    include/the_Foundation/future.h
    include/the_Foundation/garbage.h
    include/the_Foundation/geometry.h
    include/the_Foundation/hash.h
    include/the_Foundation/list.h
    include/the_Foundation/map.h
    include/the_Foundation/math.h
    include/the_Foundation/math_${mathSpec}.h
    include/the_Foundation/mutex.h
    include/the_Foundation/noise.h
    include/the_Foundation/object.h
    include/the_Foundation/objectlist.h
    include/the_Foundation/path.h
    include/the_Foundation/process.h
    include/the_Foundation/ptrarray.h
    include/the_Foundation/ptrset.h
    include/the_Foundation/rect.h
    include/the_Foundation/queue.h
    include/the_Foundation/random.h
    include/the_Foundation/range.h
    include/the_Foundation/service.h
    include/the_Foundation/socket.h
    include/the_Foundation/sortedarray.h
    include/the_Foundation/stdthreads.h
    include/the_Foundation/stream.h
    include/the_Foundation/string.h
    include/the_Foundation/stringarray.h
    include/the_Foundation/stringhash.h
    include/the_Foundation/stringlist.h
    include/the_Foundation/thread.h
    include/the_Foundation/threadpool.h
    include/the_Foundation/time.h
    include/the_Foundation/treenode.h
    include/the_Foundation/vec2.h
    include/the_Foundation/version.h
)
set (SOURCES
    src/the_foundation.c
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
    src/fixedpoint.c
    src/future.c
    src/garbage.c
    src/geometry.c
    src/hash.c
    src/list.c
    src/map.c
    src/md5.c
    src/mutex.c
    src/math.c
    src/math_${mathSpec}.c
    src/noise.c
    src/object.c
    src/objectlist.c
    src/path.c
    src/ptrarray.c
    src/ptrset.c
    src/random.c
    src/rect.c
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
if (NOT iHaveC11Threads)
    list (APPEND SOURCES src/c11threads.c)
endif ()
if (APPLE)
    set (iPlatformApple YES)
    set (SOURCES ${SOURCES} src/platform/apple.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set (iPlatformLinux YES)
    set (SOURCES ${SOURCES} src/platform/linux.c)
elseif (WIN32)
    set (iPlatformWindows YES)
    set (SOURCES ${SOURCES} src/platform/windows.c)
elseif (CMAKE_SYSTEM MATCHES "^MSYS.*")
    set (iPlatformMsys YES)
    set (SOURCES ${SOURCES} src/platform/windows.c)
elseif (CMAKE_SYSTEM MATCHES "^CYGWIN.*")
    set (iPlatformCygwin YES)
    set (SOURCES ${SOURCES} src/platform/windows.c)
else ()
    set (iPlatformOther YES)
    set (SOURCES ${SOURCES} src/platform/generic.c)
endif ()
if (NOT iPlatformWindows) # POSIX
    list (APPEND HEADERS
        src/platform/posix/pipe.h
    )
    list (APPEND SOURCES
        src/platform/posix/address.c
        src/platform/posix/datagram.c
        src/platform/posix/locale.c
        src/platform/posix/pipe.c
        src/platform/posix/process.c
        src/platform/posix/service.c
        src/platform/posix/socket.c
    )
else () # WIN32
    list (APPEND HEADERS
        src/platform/win32/msdirent.h
    )
    list (APPEND SOURCES
        src/platform/win32/address.c
        src/platform/win32/datagram.c
        src/platform/win32/process.c
        src/platform/win32/service.c
        src/platform/win32/socket.c
    )
endif ()
if (PCRE_FOUND)
    list (APPEND SOURCES src/regexp.c)
    list (APPEND HEADERS include/the_Foundation/regexp.h)
endif ()
if (CURL_FOUND)
    list (APPEND SOURCES src/webrequest.c)
    list (APPEND HEADERS include/the_Foundation/webrequest.h)
endif ()
configure_file (config.h.in config.h)

set (TARGETNAME the_Foundation)
add_library (${TARGETNAME} SHARED ${SOURCES} ${HEADERS})
if (iPlatformMsys)
    set_target_properties (${TARGETNAME} PROPERTIES
        OUTPUT_NAME       Foundation
        OUTPUT_NAME_DEBUG Foundationd
    )
else ()
    set_target_properties (${TARGETNAME} PROPERTIES
        OUTPUT_NAME       _Foundation
        OUTPUT_NAME_DEBUG _Foundationd
    )
endif ()

target_compile_definitions (${TARGETNAME} PRIVATE iIsLibraryBuild)
target_include_directories (${TARGETNAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
    ${UNISTRING_INCLUDE_DIRS}
)
tfdn_set_compile_options (${TARGETNAME})
if (iPlatformLinux OR iPlatformMsys)
    target_compile_definitions (${TARGETNAME} PRIVATE -D_GNU_SOURCE=1)
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options (${TARGETNAME} PRIVATE -fPIC)
endif ()

# Dependencies.
macro (tfdn_link_depends target mode)
    target_link_libraries (${target} ${mode} ${UNISTRING_LIBRARIES} pthread m)
    if (ZLIB_FOUND)
        target_link_libraries (${target} ${mode} ${ZLIB_LIBRARIES})
    endif ()
    if (PCRE_FOUND)
        target_link_libraries (${target} ${mode} ${PCRE_LIBRARIES})
    endif ()
    if (CURL_FOUND)
        target_link_libraries (${target} ${mode} ${CURL_LIBRARIES})
    endif ()
endmacro ()

tfdn_link_depends (${TARGETNAME} PUBLIC)

# Installation.
install (TARGETS ${TARGETNAME}
    EXPORT ${TARGETNAME}
    DESTINATION lib
)
install (EXPORT ${TARGETNAME}
    DESTINATION lib/cmake/${TARGETNAME}
    NAMESPACE the_Foundation::
    FILE ${TARGETNAME}Config.cmake
)
install (DIRECTORY include/the_Foundation
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION include/the_Foundation)

#----------------------------------------------------------------------------------------

macro (tfdn_add_test target sources)
    add_executable (${target} ${sources})
    target_link_libraries (${target} PRIVATE ${TARGETNAME})
    tfdn_set_compile_options (${target})
endmacro ()

tfdn_add_test (test_Foundation      tests/t_test.c)
tfdn_add_test (string_Foundation    tests/t_string.c)
tfdn_add_test (threading_Foundation tests/t_threading.c)
tfdn_add_test (math_Foundation      tests/t_math.c)
tfdn_add_test (network_Foundation   tests/t_network.c)
tfdn_add_test (udptest_Foundation   tests/t_udptest.c)
