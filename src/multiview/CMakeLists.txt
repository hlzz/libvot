file(GLOB_RECURSE multiview_files_header *.h)
file(GLOB_RECURSE multiview_files_cpp *.cpp)

if(LIBVOT_BUILD_SHARED)
	add_library(vot_multiview SHARED ${multiview_files_header} ${multiview_files_cpp})
elseif(LIBVOT_BUILD_SHARED)
	add_library(vot_multiview STATIC ${multiview_files_header} ${multiview_files_cpp})
endif(LIBVOT_BUILD_SHARED)

if(LIBVOT_USE_CERES)
	target_link_libraries(vot_multiview vot_numeric ${CERES_LIBRARIES})
else(LIBVOT_USE_CERES)
	target_link_libraries(vot_multiview vot_numeric)
endif(LIBVOT_USE_CERES)

set_target_properties(vot_multiview PROPERTIES SOVERSION ${LIBVOT_VERSION_MAJOR}
VERSION "${LIBVOT_VERSION_MAJOR}.${LIBVOT_VERSION_MINOR}")

install(TARGETS vot_multiview DESTINATION lib CONFIGURATIONS Release)
