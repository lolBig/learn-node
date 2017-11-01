cmake_minimum_required(VERSION 2.8)
project(ex)

include_directories(libuv-cmake/libuv/include)
add_library(ex STATIC header-parser.c structure.c)