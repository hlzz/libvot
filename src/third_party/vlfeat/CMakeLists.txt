# Skeleton of VLFeat (0.9.20)
# Tianwei Shen <shentianweipku@gmail.com>

# Change Log:
# * May 24, 2016 
# * Remove executables, exmaples, data, doc, etc.
# * Add CMake import

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)
PROJECT(vlfeat)

# MACOSX_RPATH is enabled by default.
set(CMAKE_MACOSX_RPATH TRUE)

# disable Intel@avx 
ADD_DEFINITIONS(-DVL_DISABLE_AVX)

FILE(GLOB_RECURSE vlfeat_header vl/*.h vl/*.tc)
FILE(GLOB_RECURSE vlfeat_cpp vl/*.c vl/*.th)

ADD_LIBRARY(vlfeat SHARED ${vlfeat_header} ${vlfeat_cpp})
SET_PROPERTY(TARGET vlfeat PROPERTY FOLDER libvot/third_party/vlfeat)
