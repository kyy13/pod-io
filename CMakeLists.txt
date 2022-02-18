# PODserializer
# Kyle J Burgess

cmake_minimum_required(VERSION 3.7)
project(PODserializer)

find_package ( ZLIB REQUIRED )
include_directories ( ${ZLIB_INCLUDE_DIR} )

option(BUILD_TESTS "build tests?" ON)

# library
add_library(
    ${PROJECT_NAME}
    SHARED
    src/PODserializer.cpp
    src/PsLoadFile.cpp
    src/PsSaveFile.cpp
    src/PsTypes.cpp
    src/PsDeflate.cpp
    src/Bytes.cpp
)

# includes
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    include
    ${ZLIB_INCLUDE_DIR}
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

# link libraries
target_link_libraries(
    ${PROJECT_NAME}
    ${ZLIB_LIBRARY}
    -static-libgcc
    -static-libstdc++
)

IF(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
ENDIF()
