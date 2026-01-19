cmake_minimum_required(VERSION 3.28)
project(tp1 C)

set(CMAKE_C_STANDARD 11)

add_executable(tp1 main.c
        lib.h
        monovar.c
        test.c
        multivar.c
        multivar.h
        utils.c
        utils.h
        analyse.c
        analyse.h)
