# find boost-python
find_package(Boost 1.55 REQUIRED python QUIET)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
elseif(Boost_FOUND)
    message(STATUS "Could not locate Boost-python, disabling python export.")
    return()
endif(Boost_FOUND)

# find python libs
FIND_PACKAGE(PythonLibs QUIET)
if (PythonLibs_FOUND)
    include_directories(${PYTHON_INCLUDE_DIRS})
elseif(PythonLibs_FOUND)
    message(STATUS "Could not locate python module, disabling python export.")
    set(LIBVOT_BUILD_PYTHON OFF)
    return()
endif (PythonLibs_FOUND)

# config libvot-python source code
file(GLOB_RECURSE PYVOT_SOURCE ${PROJECT_SOURCE_DIR}/python/*.cpp)

add_library(pyvot SHARED ${PYVOT_SOURCE})
target_link_libraries(pyvot ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
set_target_properties(pyvot PROPERTIES PREFIX "" OUTPUT_NAME "_libvot" SUFFIX ".so")		# this is the internal python lib

if(UNIX OR APPLE)
    set(__linkname "${PROJECT_SOURCE_DIR}/python/pyvot/_libvot.so")		# has to be .so regardless of apple or linux
    add_custom_command(TARGET pyvot POST_BUILD
                       COMMAND ln -sf $<TARGET_LINKER_FILE:pyvot> "${__linkname}"
                       COMMENT "Creating symlink ${__linkname} -> ${PROJECT_BINARY_DIR}/lib/_libvot.so")
endif()

# installation rules
file(GLOB files pyvot/*.py)
install(FILES ${files} DESTINATION python/pyvot)
install(TARGETS pyvot DESTINATION python/pyvot)
