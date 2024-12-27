cmake_minimum_required(VERSION 3.31)

project(filethrough VERSION 0.1.0)

add_executable(tft_client tft_client.c)
add_executable(tft_server tft_server.c)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED true)

target_compile_options(tft_client PRIVATE -Wall -Werror -O2 -g3 -fpic -std=c17 -pedantic)
target_compile_options(tft_server PRIVATE -Wall -Werror -O2 -g3 -fpic -std=c17 -pedantic)

target_link_libraries(tft_client PRIVATE stoml.so)
target_link_libraries(tft_server PRIVATE stoml.so)
