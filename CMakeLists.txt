cmake_minimum_required(VERSION 3.10)

project(tofkel LANGUAGES C)

add_executable(tofkel main.c)

target_include_directories(tofkel PRIVATE include)
