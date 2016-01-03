# Copyright (c) 2015 Tianwei Shen (shentianweipku@gmail.com)

cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

project(libvot)

# set compiler options, output all warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()

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

# add gtest 1.7.0
add_subdirectory(gtest-1.7.0)
add_subdirectory(test)