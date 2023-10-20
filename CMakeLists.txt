# pod-io
# Kyle J Burgess

cmake_minimum_required(VERSION 3.7)
project(pod-io)

option(POD_BUILD_SHARED "build shared?" ON)
option(POD_BUILD_TESTS "build tests?" ON)
option(POD_BUILD_ZLIB "build zlib?" ON)
option(POD_STATIC_ZLIB "link statically to zlib?" ON)
set(POD_ZLIB_LOCATION "" CACHE FILEPATH "location of zlib *.a/*.dll/*.so if POD_BUILD_ZLIB=OFF")
set(POD_ZLIB_IMPLIB "" CACHE FILEPATH "location of zlib *.lib/*.dll.a if POD_BUILD_ZLIB=OFF")
set(POD_ZLIB_INCLUDE "ext/zlib" CACHE PATH "location of zlib headers if POD_BUILD_ZLIB=OFF")

if (${POD_BUILD_ZLIB})
    add_subdirectory(ext/zlib)
else()
    if (${POD_STATIC_ZLIB})
        add_library(zlib STATIC IMPORTED GLOBAL)
    else()
        add_library(zlib SHARED IMPORTED GLOBAL)
    endif()
    set_target_properties(
        zlib PROPERTIES
        IMPORTED_LOCATION ${POD_ZLIB_LOCATION}
        IMPORTED_IMPLIB ${POD_ZLIB_IMPLIB}
        INTERFACE_INCLUDE_DIRECTORIES ${POD_ZLIB_INCLUDE}
    )
endif()

set(SOURCES
    src/pod_io.cpp
    src/PodLoadFile.cpp
    src/PodSaveFile.cpp
    src/PodDeflate.cpp
    src/PodBytes.cpp
    src/PodFile.cpp
)

# library
if (${POD_BUILD_SHARED})
    add_library(
        ${PROJECT_NAME}
        SHARED
        ${SOURCES}
    )
else()
    add_library(
        ${PROJECT_NAME}
        STATIC
        ${SOURCES}
    )
endif()

# includes
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    include
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    include
    src
)

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
        -Wall
        -g
    )
ELSE()
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
        -O3
    )
ENDIF()

# c++ version
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

if (${POD_STATIC_ZLIB})
    set(ZLIB_LIBRARY zlibstatic)
else()
    set(ZLIB_LIBRARY zlib)
endif()

# link libraries
target_link_libraries(
    ${PROJECT_NAME}
    ${ZLIB_LIBRARY}
    -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
)

IF(POD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
ENDIF()
