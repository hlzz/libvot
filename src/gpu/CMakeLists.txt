set(GPU_INCLUDE .)
set(SYS_INCLUDE )

if(LIBVOT_USE_CUDA)
	# GPU lib is going to be header-file only.
	list(APPEND GPU_INCLUDE
		cuda_utils.h)

	cuda_add_executable(trainlenet lenet.cu readubyte.cpp)
	cuda_add_cublas_to_target(trainlenet)
	target_link_libraries(trainlenet ${GFLAGS_LIBRARY} ${CUDA_LINKER_LIBS} vot_utils)
	install (TARGETS trainlenet DESTINATION bin CONFIGURATIONS Release)

	cuda_add_executable(rnn_example rnn_example.cu)
	cuda_add_cublas_to_target(rnn_example)
	target_link_libraries(rnn_example ${GFLAGS_LIBRARY} ${CUDA_LINKER_LIBS} vot_utils)
	install (TARGETS rnn_example DESTINATION bin CONFIGURATIONS Release)

	# marvin library
	cuda_add_executable(marvin marvin.hpp marvin.cu ${GPU_INCLUDE})
	cuda_add_cublas_to_target(marvin)
	target_link_libraries(marvin pthread ${CUDA_LINKER_LIBS})
	install (TARGETS marvin DESTINATION bin CONFIGURATIONS Release)
endif(LIBVOT_USE_CUDA)
