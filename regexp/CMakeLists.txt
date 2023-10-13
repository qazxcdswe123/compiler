cmake_minimum_required(VERSION 3.26)
project(regexp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# TODO: implement
#add_library(libregexp)

add_executable(regexp_test test/regexp_test.cpp)
target_link_libraries(regexp_test GTest::gtest_main)
#target_link_libraries(regexp_test libregexp)