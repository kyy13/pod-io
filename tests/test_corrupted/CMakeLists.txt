# pod-index
# Kyle J Burgess

add_executable(
    test_corrupted
    src/main.cpp
)

target_include_directories(
    test_corrupted
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(
        test_corrupted
        PRIVATE
        -Wall
        -g
    )
ELSE()
    target_compile_options(
        test_corrupted
        PRIVATE
        -O3
    )
ENDIF()

target_link_libraries(
    test_corrupted
    ${PROJECT_NAME}
    -Wl,-allow-multiple-definition
)

add_test(
    NAME
    test_corrupted
    COMMAND
    test_corrupted
)

set_target_properties(
    test_corrupted
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
