ADD_SUBDIRECTORY(vlfeat)
ADD_SUBDIRECTORY(gflags)

# gtest framework
IF(LIBVOT_BUILD_TESTS)
    # build GTEST
    SET(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject")
    # do not build GMOCK
    SET(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")
    ADD_SUBDIRECTORY(gtest)
ENDIF(LIBVOT_BUILD_TESTS)
