include_directories(${gtest_SOURCE_DIR}/include)

set(test_sources 
unit_test.cpp 
../src/image_graph/image_graph.cpp 
../src/utils/data_structures.cpp)

add_executable(unit_test ${test_sources})
target_link_libraries(unit_test gtest_main)