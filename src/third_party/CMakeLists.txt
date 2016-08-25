add_subdirectory(vlfeat)
add_subdirectory(gflags)
add_subdirectory(vectorGraphics)

# gtest framework
if(LIBVOT_BUILD_TESTS)
    # build GTEST
    set(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject")
    # do not build GMOCK
    set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")
    add_subdirectory(gtest)
endif(LIBVOT_BUILD_TESTS)
