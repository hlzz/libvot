cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# include files
set(LIBVOT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/vocab_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/examples
    ${CMAKE_CURRENT_SOURCE_DIR}/utils)
include_directories(${LIBVOT_INCLUDES})

# utility functions
add_subdirectory(utils)

# the main vocabulary tree directory
add_subdirectory(vocab_tree)

# the executable files
add_subdirectory(examples)
