cmake_minimum_required(VERSION 3.26)
project(TinyHttpServer C)

set(CMAKE_C_STANDARD 11)

add_executable(TinyHttpServer
        src/http_handler.c
        include/http_handler.h
        src/main.c)
