#
# Copyright (c) 2024 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_dect_1)

target_sources(app PRIVATE src/main.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE all_sources
    "src/*.c"
    "src/*.cpp"
    "src/decthandler/*.cpp"
    "src/decthandler/*.c"
)

target_sources(app PRIVATE ${all_sources})
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/decthandler)





file(GLOB_RECURSE all_include_dirs LIST_DIRECTORIES true "src/*")
foreach(dir ${all_include_dirs})
    if(IS_DIRECTORY ${dir})
        target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    endif()
endforeach()

