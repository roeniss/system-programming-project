cmake_minimum_required(VERSION 3.15)

project(project1 C)

add_compile_options(-Wall)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /Users/roeniss/CLionProjects/system-programming-project/)

set(SRC_FILES 20140424.c lib/history.c lib/optab.c lib/assemble.c lib/others.h lib/others.c lib/vm.h lib/vm.c)

add_executable(20140424.out ${SRC_FILES})