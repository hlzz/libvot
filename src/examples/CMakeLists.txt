set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# compile options for vocabulary tree example (need c++11)
add_executable(image_search image_search.cpp ../vocab_tree/vot_pipeline.cpp ../vocab_tree/vocab_tree.cpp
../vocab_tree/clustering.cpp ../utils/global_params.cpp ../utils/io_utils.cpp ../utils/data_structures.cpp)
set_property(TARGET image_search PROPERTY CXX_STANDARD 11)
set_property(TARGET image_search PROPERTY CXX_STANDARD_REQUIRED on)
target_link_libraries(image_search ${CMAKE_THREAD_LIBS_INIT})
install (TARGETS image_search DESTINATION bin CONFIGURATIONS Release)

add_executable(graph_partition graph_partition.cpp ../image_graph/image_graph.cpp ../utils/io_utils.cpp
../utils/global_params.cpp ../utils/data_structures.cpp)
set_property(TARGET graph_partition PROPERTY CXX_STANDARD 11)
set_property(TARGET graph_partition PROPERTY CXX_STANDARD_REQUIRED on)
install (TARGETS graph_partition DESTINATION bin CONFIGURATIONS Release)

add_executable(query_expansion query_expansion.cpp ../utils/io_utils.cpp ../image_graph/image_graph.cpp
../utils/data_structures.cpp)
set_property(TARGET query_expansion PROPERTY CXX_STANDARD 11)
set_property(TARGET query_expansion PROPERTY CXX_STANDARD_REQUIRED on)
install (TARGETS query_expansion DESTINATION bin CONFIGURATIONS Release)

add_executable(sequence_match sequence_match.cpp ../utils/io_utils.cpp)
set_property(TARGET sequence_match PROPERTY CXX_STANDARD 11)
set_property(TARGET sequence_match PROPERTY CXX_STANDARD_REQUIRED on)
install (TARGETS sequence_match DESTINATION bin CONFIGURATIONS Release)

add_executable(compute_stats compute_stats.cpp ../utils/io_utils.cpp)
set_property(TARGET compute_stats PROPERTY CXX_STANDARD 11)
set_property(TARGET compute_stats PROPERTY CXX_STANDARD_REQUIRED on)
install (TARGETS compute_stats DESTINATION bin CONFIGURATIONS Release)

add_executable(svt svt.cpp ../utils/io_utils.cpp)
set_property(TARGET svt PROPERTY CXX_STANDARD 11)
set_property(TARGET svt PROPERTY CXX_STANDARD_REQUIRED on)
install (TARGETS svt DESTINATION bin CONFIGURATIONS Release)

add_executable(euclidean_matrix euclidean_matrix.cpp ../utils/io_utils.cpp)
set_property(TARGET euclidean_matrix PROPERTY CXX_STANDARD 11)
set_property(TARGET euclidean_matrix PROPERTY CXX_STANDARD_REQUIRED on)
install (TARGETS euclidean_matrix DESTINATION bin CONFIGURATIONS Release)

add_executable(web_search web_search.cpp ../vocab_tree/vocab_tree.cpp ../vocab_tree/clustering.cpp
../utils/global_params.cpp ../utils/io_utils.cpp ../utils/data_structures.cpp)
set_property(TARGET web_search PROPERTY CXX_STANDARD 11)
set_property(TARGET web_search PROPERTY CXX_STANDARD_REQUIRED on)
install (TARGETS web_search DESTINATION bin CONFIGURATIONS Release)

# using vlfeat feature in libvot
add_executable(libvot_vlfeat libvot_vlfeat.cpp)
target_link_libraries(libvot_vlfeat ${GFLAGS_LIBRARY} vlfeat_vot)
install (TARGETS libvot_vlfeat DESTINATION bin CONFIGURATIONS Release)
