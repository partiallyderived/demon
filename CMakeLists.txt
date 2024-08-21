cmake_minimum_required(VERSION 3.12)
project(Demon VERSION 0.1.0)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib )

find_package(Catch2 3 REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
    ${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_executable(test-lex test/test_lex.cpp)
add_executable(test-tokens test/test_tokens.cpp)

target_link_libraries(
    test-lex PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME}
)
target_link_libraries(
    test-tokens PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME}
)
