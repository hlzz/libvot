# Copyright (c) 2015 - 2016. Tianwei Shen (shentianweipku@gmail.com)
# Hong Kong University of Science and Technology (HKUST)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)  # newest gflags needs 2.8.12

project(libvot CXX)

# Disable in-source build
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source build is not allowed, please do out-of-source build.")
endif()

# The version number.
set(LIBVOT_VERSION_MAJOR 0)
set(LIBVOT_VERSION_MINOR 1)
set(LIBVOT_VERSION_PATCH 3)
set(LIBVOT_VERSION ${LIBVOT_VERSION_MAJOR}.${LIBVOT_VERSION_MINOR}.${LIBVOT_VERSION_PATCH})

# set compiler options, output all warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()

# ================= libvot build options =============================
option(LIBVOT_BUILD_TESTS "Build libvot tests" ON)
option(LIBVOT_BUILD_SHARED "Build libvot shared libs" ON)
option(LIBVOT_BUILD_DOC "Build libvot documentations" ON)
option(LIBVOT_USE_OPENCV "Build libvot with OpenCV support" ON)


# ================= Configure libvot options =========================
if(LIBVOT_USE_OPENCV)
    find_package( OpenCV REQUIRED )
    if (OpenCV_FOUND)
        include_directories(${OpenCV_INCLUDE_DIRS})
		add_definitions(-DLIBVOT_USE_OPENCV)
    elseif(OpenCV_FOUND)
        message(STATUS "Could not locate OpenCV, disabling OpenCV support.")
        set(LIBVOT_USE_OPENCV OFF)
		remove_definitions(-DLIBVOT_USE_OPENCV)
    endif (OpenCV_FOUND)
endif()

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

# ==============================================================================
# Set includes and libraries begin
# ==============================================================================
# (This has to be specified before compiling the executables)
# include files
set(LIBVOT_INCLUDES
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/third_party
    ${CMAKE_SOURCE_DIR}/src/vocab_tree
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/feature
    ${CMAKE_SOURCE_DIR}/src/matching
    )
include_directories(BEFORE ${LIBVOT_INCLUDES})

# Setup vlfeat for feature detection
set(VLFEAT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src/third_party/vlfeat
    )
set(VLFEAT_LIBRARY vlfeat)
include_directories(BEFORE ${VLFEAT_INCLUDE_DIRS})

# Setup gflags for command line tools
# use the built-in gflags include directory, defined in gflags CMakefile
include_directories(BEFORE ${GFLAGS_INCLUDE_DIR})
set(GFLAGS_LIBRARY gflags)
# =============================== Set libraries end ==============================

# The source files
add_subdirectory(src)

# Test source code
if(LIBVOT_BUILD_TESTS)
    enable_testing()
	add_subdirectory(test)
endif(LIBVOT_BUILD_TESTS)

# ================= Output building configurations =========================
message("\n")
message("Building configurations:\n")
message("** libvot version: " ${LIBVOT_VERSION})
message("** Build shared libs: " ${LIBVOT_BUILD_SHARED})
message("** Build tests: " ${LIBVOT_BUILD_TESTS})
message("** Build docs: " ${LIBVOT_BUILD_DOC})
message("** Build type: " ${CMAKE_BUILD_TYPE})
if(LIBVOT_USE_OPENCV)
    message("** Compile with OpenCV, headers found in " ${OpenCV_INCLUDE_DIRS} ".")
endif(LIBVOT_USE_OPENCV)
message("\n")
