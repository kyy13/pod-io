# pod-index
# Kyle J Burgess

add_executable(
    test_checksum
    src/main.cpp
)

target_include_directories(
    test_checksum
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(
        test_checksum
        PRIVATE
        -Wall
        -g
    )
ELSE()
    target_compile_options(
        test_checksum
        PRIVATE
        -O3
    )
ENDIF()

target_link_libraries(
    test_checksum
    ${PROJECT_NAME}
    -Wl,-allow-multiple-definition
)

add_test(
    NAME
    test_checksum
    COMMAND
    test_checksum
)

set_target_properties(
    test_checksum
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
