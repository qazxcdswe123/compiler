cmake_minimum_required(VERSION 3.26)
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

add_executable(regex_main main.cpp)
target_link_libraries(regex_main libregexp)

add_executable(regex_server server.cpp)
target_link_libraries(regex_server libregexp)
