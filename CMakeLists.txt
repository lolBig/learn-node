cmake_minimum_required(VERSION 2.8)
project(learnuv)

include_directories(libuv-cmake/libuv/include)
add_subdirectory(libuv-cmake)
add_executable(ssdp src/ssdp.c)
target_link_libraries(ssdp uv)