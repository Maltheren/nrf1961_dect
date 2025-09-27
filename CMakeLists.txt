#
# Copyright (c) 2024 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_dect_1)

target_sources(app PRIVATE src/main.c)



file(GLOB app_sources
    "src/applications/*.c"
    "src/applications/*.cpp"
)
target_sources(app PRIVATE ${app_sources})

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/applications)
