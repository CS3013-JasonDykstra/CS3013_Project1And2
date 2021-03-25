cmake_minimum_required(VERSION 3.16.3)
project(CS3013Project2 C)

set(CMAKE_C_STANDARD 99)

add_executable(CS3013Project2 problem1.c)

find_package(Threads REQUIRED)
target_link_libraries(CS3013Project2 Threads::Threads)