cmake_minimum_required(VERSION 3.18)
project(grammar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(libgrammar
        grammar.h
        grammar.cpp
)

add_executable(grammar_server
        server.cpp
)
target_link_libraries(grammar_server libgrammar)

add_executable(grammar_test test/grammar_test.cpp)
target_link_libraries(grammar_test libgrammar)
target_link_libraries(grammar_test gtest_main)