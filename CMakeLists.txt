# pod-index
# Kyle J Burgess

cmake_minimum_required(VERSION 3.7)
project(podindex)

option(POD_BUILD_SHARED "build shared?" ON)
option(POD_BUILD_TESTS "build tests?" ON)
option(POD_BUILD_ZLIB "build zlib?" ON)
option(POD_STATIC_ZLIB "link statically to zlib?" ON)

if (${POD_BUILD_ZLIB})
    add_subdirectory(ext/zlib)
endif()

set(SOURCES
    src/pod_index.cpp
    src/PxLoadFile.cpp
    src/PxSaveFile.cpp
    src/PxDeflate.cpp
    src/PxBytes.cpp
    src/PxFile.cpp
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
    -static-libgcc
    -static-libstdc++
    -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
)

IF(POD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
ENDIF()
