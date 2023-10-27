cmake_minimum_required(VERSION 3.18)
project(lexer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(liblexer
        lexer.cpp
        include/lexer.h)

add_executable(lexer_cli
        main.cpp
)
target_link_libraries(lexer_cli liblexer)

add_executable(lexer_http
        server.cpp
)
target_link_libraries(lexer_http pthread liblexer)

add_executable(lexer_test test/lexer_test.cpp)
target_link_libraries(lexer_test GTest::gtest_main)
target_link_libraries(lexer_test liblexer)