cmake_minimum_required(VERSION 3.28)
project(tp1 C)

set(CMAKE_C_STANDARD 11)

add_executable(tp1 main.c
        lib.h
        utils.c
        test.c)
