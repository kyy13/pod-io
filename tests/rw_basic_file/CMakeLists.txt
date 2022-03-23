# pod-index
# Kyle J Burgess

add_executable(
    rw_basic_file
    src/main.cpp
)

target_include_directories(
    rw_basic_file
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(
        rw_basic_file
        PRIVATE
        -Wall
        -g
    )
ELSE()
    target_compile_options(
        rw_basic_file
        PRIVATE
        -O3
    )
ENDIF()

target_link_libraries(
    rw_basic_file
    ${PROJECT_NAME}
    -Wl,-allow-multiple-definition
)

add_test(
    NAME
    rw_basic_file
    COMMAND
    rw_basic_file
)

set_target_properties(
    rw_basic_file
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
