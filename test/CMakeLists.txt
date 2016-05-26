# Tianwei Shen <shentianweipku@gmail.com> HKUST.
# All rights reserved.

# Where Google Test's .h files can be found.
INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include)
# Adds Google Mock's and Google Test's header directories to the search path.
INCLUDE_DIRECTORIES("${gmock_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}"
                    "${gtest_SOURCE_DIR}/include"
                    # This directory is needed to build directly from Google
                    # Test sources.
                    "${gtest_SOURCE_DIR}")

SET(THREADS_PREFER_PTHREAD_FLAG ON)
FIND_PACKAGE(Threads REQUIRED)

set(TEST_SOURCES 
unit_test.cpp 
../src/image_graph/image_graph.cpp 
../src/utils/data_structures.cpp
../src/vocab_tree/vot_pipeline.cpp
../src/vocab_tree/clustering.cpp
../src/utils/global_params.cpp
../src/utils/io_utils.cpp
../src/vocab_tree/vocab_tree.cpp)

# compile options for unit test (need c++11)
ADD_EXECUTABLE(unit_test ${TEST_SOURCES})
SET_PROPERTY(TARGET unit_test PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET unit_test PROPERTY CXX_STANDARD_REQUIRED ON)
# target_link_libraries(unit_test Threads::Threads)		# this will fail on travis-CI
TARGET_LINK_LIBRARIES(unit_test ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(unit_test gtest_main gmock_main)

ADD_TEST(unit_test unit_test)
