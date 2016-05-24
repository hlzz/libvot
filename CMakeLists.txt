# Copyright (c) 2015 - 2016. Tianwei Shen (shentianweipku@gmail.com)
# Hong Kong University of Science and Technology (HKUST)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)

PROJECT(libvot)

# Disable in-source build
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source build is not allowed, please do out-of-source build.")
ENDIF()

#------------------------------------------------------------------
# set policy

# MACOSX_RPATH is enabled by default.
set(CMAKE_MACOSX_RPATH TRUE)

#------------------------------------------------------------------

# set compiler options, output all warnings
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(warnings "-Wall -Wextra -Werror")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    SET(warnings "/W4 /WX /EHsc")
ENDIF()

# libvot build options
OPTION(LIBVOT_BUILD_TESTS "Build libvot tests" OFF)

# by default build in release mode
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

# Additional cmake modules
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR/cmake}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# The source files
ADD_SUBDIRECTORY(src)

# add gtest 1.7.0
ADD_SUBDIRECTORY(gtest-1.7.0)
ADD_SUBDIRECTORY(test)