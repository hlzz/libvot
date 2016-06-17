cuda_add_executable(trainlenet lenet.cu readubyte.cpp)
cuda_add_cublas_to_target(trainlenet)
target_link_libraries(trainlenet ${GFLAGS_LIBRARY} ${CUDA_LINKER_LIBS})
