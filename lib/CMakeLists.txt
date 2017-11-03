cmake_minimum_required(VERSION 2.8)
project(learnlib)

include_directories(libuv-cmake/libuv/include)
add_library(learnlib STATIC header-parser.c structure.c)