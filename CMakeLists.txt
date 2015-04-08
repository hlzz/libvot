# Copyright (c) 2015 Tianwei Shen (shentianweipku@gmail.com)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(libvot)

# libvot build options
option(LIBVOT_BUILD_TESTS "Build libvot tests" OFF)

# by default build in release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

# Additional cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR/cmake}")

# The source files
add_subdirectory(src)
