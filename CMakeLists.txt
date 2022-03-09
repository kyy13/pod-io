# PODstore
# Kyle J Burgess

cmake_minimum_required(VERSION 3.7)
project(PODstore)

add_subdirectory(ext/zlib)

option(BUILD_TESTS "build tests?" ON)

# library
add_library(
    ${PROJECT_NAME}
    SHARED
    src/PODstore.cpp
    src/PsLoadFile.cpp
    src/PsSaveFile.cpp
    src/PsDeflate.cpp
    src/PsBytes.cpp
    src/PsFile.cpp
)

# includes
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    include
    ext/k13/include
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
    zlibstatic
    -static-libgcc
    -static-libstdc++
)

IF(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
ENDIF()
