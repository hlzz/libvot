# Tianwei Shen <shentianweipku@gmail.com> HKUST.
# All rights reserved.

# Where Google Test's .h files can be found.
include_directories(${gtest_SOURCE_DIR}/include)
# Adds Google Mock's and Google Test's header directories to the search path.
include_directories("${gmock_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}"
                    "${gtest_SOURCE_DIR}/include"
                    # This directory is needed to build directly from Google
                    # Test sources.
                    "${gtest_SOURCE_DIR}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(test_sources 
unit_test.cpp 
../src/image_graph/image_graph.cpp 
../src/utils/data_structures.cpp
../src/vocab_tree/vot_pipeline.cpp
../src/vocab_tree/clustering.cpp
../src/utils/global_params.cpp
../src/utils/io_utils.cpp
../src/vocab_tree/vocab_tree.cpp)

# compile options for unit test (need c++11)
add_executable(unit_test ${test_sources})
set_property(TARGET unit_test PROPERTY CXX_STANDARD 11)
set_property(TARGET unit_test PROPERTY CXX_STANDARD_REQUIRED on)
# target_link_libraries(unit_test Threads::Threads)		# this will fail on travis-CI
target_link_libraries(unit_test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(unit_test gtest_main gmock_main)

