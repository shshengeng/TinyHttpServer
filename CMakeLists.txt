cmake_minimum_required(VERSION 3.26)
project(TinyHttpServer C)

set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(TinyHttpServer
        src/http_handler.c
        include/http_handler.h
        src/main.c)
