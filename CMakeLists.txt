cmake_minimum_required(VERSION 3.10)

project(tofkel LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(_GNU_SOURCE)

add_compile_options(-std=c23 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined)

add_link_options(-fsanitize=address,undefined)

add_subdirectory(src)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
