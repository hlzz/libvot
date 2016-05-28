if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # MACOSX_RPATH is enabled by default.
endif()

file(
    GLOB_RECURSE
    VOT_FEATURE_HEADER
    *.h
    *.hpp
)
file(
    GLOB_RECURSE
    VOT_FEATURE_SOURCE
    *.cpp
)

if(LIBVOT_BUILD_SHARED)
    add_library(vlfeat_vot SHARED ${VOT_FEATURE_HEADER} ${VOT_FEATURE_SOURCE})
else(LIBVOT_BUILD_SHARED)
    add_library(vlfeat_vot STATIC ${VOT_FEATURE_HEADER} ${VOT_FEATURE_SOURCE})
endif(LIBVOT_BUILD_SHARED)
target_link_libraries (vlfeat_vot ${VLFEAT_LIBRARY})
