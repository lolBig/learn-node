cmake_minimum_required(VERSION 2.8)
project(learn-node)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

include_directories(libuv-cmake/libuv/include)
include_directories(/usr/local/opt/openssl/include/)
link_libraries(/usr/local/opt/openssl/lib)

add_subdirectory(libuv-cmake)

include_directories(lib)
add_subdirectory(lib)

add_executable(ssdp src/ssdp.c)
target_link_libraries(ssdp uv)

add_executable(wget src/wget.c)
target_link_libraries(wget uv ex ssl crypto)

add_executable(websocket src/websocket.c)
target_link_libraries(websocket uv)

add_executable(simple-producer-consumer src/simple-producer-consumer.c)
target_link_libraries(simple-producer-consumer)
