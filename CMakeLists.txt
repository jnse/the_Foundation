cmake_minimum_required (VERSION 3.1)
project (the_Foundation VERSION 1.0.0 LANGUAGES C)
include (CheckIncludeFile)
include (CheckCSourceCompiles)
include (TestBigEndian)
find_package (PkgConfig)

macro (tfdn_set_compile_options target)
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

option (TFDN_ENABLE_SSE41 "Enable SSE 4.1 instructions" ON)
option (TFDN_ENABLE_DEBUG_OUTPUT "Enable internal debug output to stdout/stderr" OFF)

if (TFDN_ENABLE_DEBUG_OUTPUT)
    set (iHaveDebugOutput YES)
endif ()

# zlib
find_package (ZLIB)
if (ZLIB_FOUND)
    set (iHaveZlib YES)
endif ()

# Regular expressions
pkg_check_modules (PCRE libpcre)
if (PCRE_FOUND)
    set (iHavePcre YES)
endif ()

# CURL
find_package (CURL)
if (CURL_FOUND)
    set (iHaveCurl YES)
endif ()

# C11 threads
check_include_file (threads.h iHaveC11Threads)
if (NOT iHaveC11Threads)
    check_c_source_compiles ("
        #include <pthread.h>
        int main(int argc, char **argv) {
            return PTHREAD_MUTEX_TIMED_NP;
        }"
    iHavePthreadTimedMutex)
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
    include/the_Foundation/future.h
    include/the_Foundation/garbage.h
    include/the_Foundation/hash.h
    include/the_Foundation/list.h
    include/the_Foundation/map.h
    include/the_Foundation/math.h
    include/the_Foundation/math_${mathSpec}.h
    include/the_Foundation/mutex.h
    include/the_Foundation/object.h
    include/the_Foundation/objectlist.h
    include/the_Foundation/path.h
    include/the_Foundation/pipe.h
    include/the_Foundation/process.h
    include/the_Foundation/ptrarray.h
    include/the_Foundation/ptrset.h
    include/the_Foundation/queue.h
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
    include/the_Foundation/version.h
)
set (SOURCES
    src/the_foundation.c
    src/address.c
    src/audience.c
    src/array.c
    src/block.c
    src/blockhash.c
    src/buffer.c
    src/class.c
    src/commandline.c
    src/crc32.c
    src/datagram.c
    src/file.c
    src/fileinfo.c
    src/future.c
    src/garbage.c
    src/hash.c
    src/list.c
    src/map.c
    src/md5.c
    src/mutex.c
    src/math.c
    src/math_${mathSpec}.c
    src/object.c
    src/objectlist.c
    src/path.c
    src/pipe.c
    src/process.c
    src/ptrarray.c
    src/ptrset.c
    src/queue.c
    src/service.c
    src/socket.c
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
else ()
    set (iPlatformOther YES)
    set (SOURCES ${SOURCES} src/platform/generic.c)
endif ()
if (PCRE_FOUND)
    set (SOURCES ${SOURCES} src/regexp.c)
    set (HEADERS ${HEADERS} include/the_Foundation/regexp.h)
endif ()
if (CURL_FOUND)
    set (SOURCES ${SOURCES} src/webrequest.c)
    set (HEADERS ${HEADERS} include/the_Foundation/webrequest.h)
endif ()
configure_file (config.h.in config.h)

set (TARGETNAME the_Foundation)
add_library (${TARGETNAME} SHARED ${SOURCES} ${HEADERS})
if (iPlatformMsys)
    set_target_properties (${TARGETNAME} PROPERTIES
        OUTPUT_NAME       Foundation
        OUTPUT_NAME_DEBUG Foundationd
    )
elseif (UNIX)
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
)
tfdn_set_compile_options (${TARGETNAME})
if (iPlatformLinux OR iPlatformMsys)
    target_compile_definitions (${TARGETNAME} PRIVATE -D_GNU_SOURCE=1)
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options (${TARGETNAME} PRIVATE -fPIC)
endif ()

# Dependencies.
target_link_libraries (${TARGETNAME} PRIVATE m pthread)
if (ZLIB_FOUND)
    target_include_directories (${TARGETNAME} PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries      (${TARGETNAME} PRIVATE ${ZLIB_LIBRARIES})
endif ()
if (PCRE_FOUND)
    target_include_directories (${TARGETNAME} PRIVATE ${PCRE_INCLUDE_DIRS})
    target_link_libraries      (${TARGETNAME} PRIVATE ${PCRE_LDFLAGS})
endif ()
if (CURL_FOUND)
    target_include_directories (${TARGETNAME} PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries      (${TARGETNAME} PRIVATE ${CURL_LIBRARIES})
endif ()

# Installation.
install (TARGETS ${TARGETNAME} EXPORT ${TARGETNAME} DESTINATION lib)
install (EXPORT ${TARGETNAME} DESTINATION lib/cmake/${TARGETNAME} NAMESPACE the_Foundation:: FILE ${TARGETNAME}Config.cmake)
install (DIRECTORY include/the_Foundation DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION include/the_Foundation)

#----------------------------------------------------------------------------------------

# Generate prototypes based on headers.
#function (tfdn_generate_prototypes headerPath)
#    get_filename_component (name ${headerPath} NAME_WE)
#    add_custom_target (tfdn_proto_${name}
#        ${CMAKE_CURRENT_LIST_DIR}/prototypes.py ${headerPath}
#        DEPENDS ${headerPath}
#        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#        COMMENT "Generating prototypes: ${name}")
#    add_dependencies (${TARGETNAME} tfdn_proto_${name})
#endfunction ()

#foreach (header ${HEADERS})
#    tfdn_generate_prototypes (${header})
#endforeach ()

#add_subdirectory (lang/c++)

#----------------------------------------------------------------------------------------

macro (tfdn_add_test target sources)
    add_executable (${target} ${sources})
    target_link_libraries (${target} PRIVATE ${TARGETNAME} m)
    tfdn_set_compile_options (${target})
endmacro ()

tfdn_add_test (test_Foundation      tests/main.c)
tfdn_add_test (strings_Foundation   tests/strings.c)
tfdn_add_test (threading_Foundation tests/threading.c)
tfdn_add_test (math_Foundation      tests/mathops.c)
tfdn_add_test (network_Foundation   tests/network.c)
tfdn_add_test (udptest_Foundation   tests/udptest.c)
