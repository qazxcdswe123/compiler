cmake_minimum_required(VERSION 3.18)
project(regexp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(libregexp
        re.h
        re.cpp
        nfa.h
        nfa.cpp
        dfa.h
        dfa.cpp
        codegen.h
        codegen.cpp
)

add_executable(regexp_test test/regexp_test.cpp)
target_link_libraries(regexp_test GTest::gtest_main libregexp)

add_executable(regexp_server server.cpp)
target_link_libraries(regexp_server pthread libregexp)
