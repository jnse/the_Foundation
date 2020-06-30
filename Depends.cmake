# Dependencies for the_Foundation
find_package (PkgConfig)

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

macro (tfdn_link_depends target mode)
    target_include_directories (${target} ${mode}
        ${UNISTRING_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
        ${PCRE_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
    )
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
