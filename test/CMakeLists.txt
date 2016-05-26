# Tianwei Shen <shentianweipku@gmail.com> HKUST.
# All rights reserved.

# Where Google Test's .h files can be found.
INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include)

SET(THREADS_PREFER_PTHREAD_FLAG ON)
FIND_PACKAGE(Threads REQUIRED)

SET(TEST_SOURCES 
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
TARGET_LINK_LIBRARIES(unit_test gtest_main)

ADD_TEST(unit_test unit_test)
