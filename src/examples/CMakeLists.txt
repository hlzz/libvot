SET(THREADS_PREFER_PTHREAD_FLAG ON)
FIND_PACKAGE(Threads REQUIRED)

# compile options for vocabulary tree example (need c++11)
ADD_EXECUTABLE(image_search image_search.cpp ../vocab_tree/vot_pipeline.cpp ../vocab_tree/vocab_tree.cpp ../vocab_tree/clustering.cpp ../utils/global_params.cpp ../utils/io_utils.cpp ../utils/data_structures.cpp)
SET_PROPERTY(TARGET image_search PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET image_search PROPERTY CXX_STANDARD_REQUIRED on)
# target_link_libraries(image_search Threads::Threads)
TARGET_LINK_LIBRARIES(image_search ${CMAKE_THREAD_LIBS_INIT})

ADD_EXECUTABLE(graph_partition graph_partition.cpp ../image_graph/image_graph.cpp ../utils/io_utils.cpp ../utils/global_params.cpp ../utils/data_structures.cpp)
SET_PROPERTY(TARGET graph_partition PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET graph_partition PROPERTY CXX_STANDARD_REQUIRED on)

ADD_EXECUTABLE(query_expansion query_expansion.cpp ../utils/io_utils.cpp ../image_graph/image_graph.cpp ../utils/data_structures.cpp)
SET_PROPERTY(TARGET query_expansion PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET query_expansion PROPERTY CXX_STANDARD_REQUIRED on)

ADD_EXECUTABLE(sequence_match sequence_match.cpp ../utils/io_utils.cpp)
SET_PROPERTY(TARGET sequence_match PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET sequence_match PROPERTY CXX_STANDARD_REQUIRED on)

ADD_EXECUTABLE(compute_stats compute_stats.cpp ../utils/io_utils.cpp)
SET_PROPERTY(TARGET compute_stats PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET compute_stats PROPERTY CXX_STANDARD_REQUIRED on)

ADD_EXECUTABLE(svt svt.cpp ../utils/io_utils.cpp)
SET_PROPERTY(TARGET svt PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET svt PROPERTY CXX_STANDARD_REQUIRED on)

ADD_EXECUTABLE(euclidean_matrix euclidean_matrix.cpp ../utils/io_utils.cpp)
SET_PROPERTY(TARGET euclidean_matrix PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET euclidean_matrix PROPERTY CXX_STANDARD_REQUIRED on)

ADD_EXECUTABLE(web_search web_search.cpp ../vocab_tree/vocab_tree.cpp ../vocab_tree/clustering.cpp ../utils/global_params.cpp ../utils/io_utils.cpp ../utils/data_structures.cpp)
SET_PROPERTY(TARGET web_search PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET web_search PROPERTY CXX_STANDARD_REQUIRED on)

ADD_EXECUTABLE(libvot_vlfeat libvot_vlfeat.cpp)
TARGET_LINK_LIBRARIES(libvot_vlfeat ${VLFEAT_LIBRARY})
