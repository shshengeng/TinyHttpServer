cmake_minimum_required(VERSION 3.10)
project(TinyHttpServer C)

set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable(TinyHttpServer ${SOURCE_FILES})
