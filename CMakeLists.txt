cmake_minimum_required(VERSION 3.13)
project(cpp_test)

set(CMAKE_C_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(pub_sub_demo sample/cpp_demo.cpp)
target_link_libraries(pub_sub_demo Threads::Threads ulog)
