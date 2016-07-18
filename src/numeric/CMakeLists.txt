file(GLOB_RECURSE numeric_files_header *.h)
file(GLOB_RECURSE numeric_files_cpp *.cpp)

if(LIBVOT_BUILD_SHARED)
	add_library(vot_numeric SHARED ${numeric_files_header} ${numeric_files_cpp})
elseif(LIBVOT_BUILD_SHARED)
	add_library(vot_numeric STATIC ${numeric_files_header} ${numeric_files_cpp})
endif(LIBVOT_BUILD_SHARED)

set_target_properties(vot_numeric PROPERTIES SOVERSION ${LIBVOT_VERSION_MAJOR}
VERSION "${LIBVOT_VERSION_MAJOR}.${LIBVOT_VERSION_MINOR}")

install(TARGETS vot_numeric DESTINATION lib CONFIGURATIONS Release)
