cmake_minimum_required(VERSION 3.15)
project(lab3 C)

include(CTest)

add_executable(lab3 main.c)
enable_testing()

set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")

target_link_libraries(lab3 -fprofile-arcs)
