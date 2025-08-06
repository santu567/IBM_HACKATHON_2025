cmake_minimum_required(VERSION 3.10)
project(MemoryAllocator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test
find_package(GTest REQUIRED)

# Add source files
set(SOURCES
    src/memory_allocator.cpp
    src/main.cpp
)

# Create main executable
add_executable(memory_allocator ${SOURCES})

# Create test executable
add_executable(unit_tests 
    test/unit_tests.cpp
    src/memory_allocator.cpp
)

# Link Google Test to test executable
target_link_libraries(unit_tests GTest::gtest GTest::gtest_main)

# Include directories
target_include_directories(memory_allocator PRIVATE src)
target_include_directories(unit_tests PRIVATE src)

# Compiler flags
target_compile_options(memory_allocator PRIVATE -Wall -Wextra -O2 -g)
target_compile_options(unit_tests PRIVATE -Wall -Wextra -O2 -g)

# Add test
enable_testing()
add_test(NAME MemoryAllocatorTests COMMAND unit_tests)

# Custom targets
add_custom_target(test-run
    COMMAND unit_tests
    DEPENDS unit_tests
    COMMENT "Running unit tests"
)

add_custom_target(build-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target memory_allocator
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target unit_tests
    COMMENT "Building all targets"
) 