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

# link pthread into vlfeat, otherwise there might be errors regarding
# pthread_key_create, pthread_getspecific, pthread_key_delete, etc.
FIND_PACKAGE(Threads REQUIRED)
IF(THREADS_HAVE_PTHREAD_ARG)
    SET_PROPERTY(TARGET vlfeat PROPERTY COMPILE_OPTIONS "-pthread")
    SET_PROPERTY(TARGET vlfeat PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
ENDIF()
IF(CMAKE_THREAD_LIBS_INIT)
    TARGET_LINK_LIBRARIES(vlfeat "${CMAKE_THREAD_LIBS_INIT}")
ENDIF()

SET_PROPERTY(TARGET vlfeat PROPERTY FOLDER libvot/third_party/vlfeat)
