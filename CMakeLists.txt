cmake_minimum_required(VERSION 3.15)

project(project1 C)

add_compile_options(-Wall)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /Users/roeniss/CLionProjects/system-programming-project/)

set(SRC_FILES 20140424.c lib/debug.c lib/dir.c lib/dump.c lib/edit.c lib/fill.c lib/help.c lib/history.c lib/myUtils.c lib/opcode.c lib/quit.c lib/reset.c)

add_executable(20140424.out ${SRC_FILES})