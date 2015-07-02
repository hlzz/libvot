#set(LIBVOT_EXAMPLES_INCLUDES
    #${CMAKE_CURRENT_SOURCE_DIR}
    #${CMAKE_CURRENT_SOURCE_DIR}/../third_party
    #${CMAKE_CURRENT_SOURCE_DIR}/../vocab_tree
    #${CMAKE_CURRENT_SOURCE_DIR}/../image_graph
    #${CMAKE_CURRENT_SOURCE_DIR}/../utils)
#include_directories(${LIBVOT_EXAMPLES_INCLUDES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# compile options for vocabulary tree example (need c++11)
add_executable(image_search image_search.cpp ../vocab_tree/vot_pipeline.cpp ../vocab_tree/vocab_tree.cpp ../vocab_tree/clustering.cpp ../utils/global_params.cpp ../utils/io_utils.cpp)
set_property(TARGET image_search PROPERTY CXX_STANDARD 11)
set_property(TARGET image_search PROPERTY CXX_STANDARD_REQUIRED on)
target_link_libraries(image_search Threads::Threads)

add_executable(graph_partition graph_partition.cpp ../image_graph/image_graph.cpp ../utils/io_utils.cpp ../utils/global_params.cpp ../utils/data_structures.cpp)

add_executable(query_expansion query_expansion.cpp ../utils/io_utils.cpp ../image_graph/image_graph.cpp ../utils/data_structures.cpp)

add_executable(sequence_match sequence_match.cpp ../utils/io_utils.cpp)

add_executable(compute_stats compute_stats.cpp ../utils/io_utils.cpp)

add_executable(svt svt.cpp ../utils/io_utils.cpp)
set_property(TARGET svt PROPERTY CXX_STANDARD 11)
set_property(TARGET svt PROPERTY CXX_STANDARD_REQUIRED on)

add_executable(euclidean_matrix euclidean_matrix.cpp ../utils/io_utils.cpp)
