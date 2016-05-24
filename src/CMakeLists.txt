CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12 FATAL_ERROR)

# ==============================================================================
# Check that submodule have been initialized and updated
# ==============================================================================
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/src/third_party/gflags)
    MESSAGE(FATAL_ERROR
        "\n submodule(s) are missing, please update your repository:\n"
        "  > git submodule update -i\n")
endif()

# include files
SET(LIBVOT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/vocab_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/examples
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    )
INCLUDE_DIRECTORIES(${LIBVOT_INCLUDES})

# ==============================================================================
# Set libraries begin 
# ==============================================================================
# (This has to be specified before compiling the executables)

# Setup vlfeat for feature detection
SET(VLFEAT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vlfeat
    )
SET(VLFEAT_LIBRARY vlfeat)
INCLUDE_DIRECTORIES(BEFORE ${VLFEAT_INCLUDE_DIRS})

# Setup gflags for command line tools
# use the built-in gflags include directory, defined in gflags CMakefile
INCLUDE_DIRECTORIES(BEFORE ${GFLAGS_INCLUDE_DIR})
SET(GFLAGS_LIBRARY gflags)

# ---------------- Set libraries end ---------------------

# the third parth libs
ADD_SUBDIRECTORY(third_party)
# utility functions
ADD_SUBDIRECTORY(utils)
# the main vocabulary tree directory
ADD_SUBDIRECTORY(vocab_tree)
# the image graph directory
ADD_SUBDIRECTORY(image_graph)
# the executable files
ADD_SUBDIRECTORY(examples)

