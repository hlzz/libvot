# Copyright (c) 2015 - 2016. Tianwei Shen (shentianweipku@gmail.com)
# Hong Kong University of Science and Technology (HKUST)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)  # newest gflags needs 2.8.12

project(libvot)

# Disable in-source build
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source build is not allowed, please do out-of-source build.")
endif()

#----------------------- Set Policy -------------------------------
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # MACOSX_RPATH is enabled by default.
endif()
#------------------------------------------------------------------

# set compiler options, output all warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()

# libvot build options
option(LIBVOT_BUILD_TESTS "Build libvot tests" ON)

# by default build in release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# Additional cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR/cmake}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# Specify output bin_path and lib_path
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# The source files
add_subdirectory(src)

# Test source code
if(LIBVOT_BUILD_TESTS)
    enable_testing()
	add_subdirectory(test)
endif(LIBVOT_BUILD_TESTS)
