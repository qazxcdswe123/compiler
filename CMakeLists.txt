cmake_minimum_required(VERSION 3.18)
project(compiler)

enable_testing()

add_subdirectory(googletest)
add_subdirectory(lexer)
add_subdirectory(regexp)
add_subdirectory(grammar)