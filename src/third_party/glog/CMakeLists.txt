set(INC src ../gflags/src)
include_directories(src)
include_directories(../gflags/src)

set(SRC
	src/logging.cc
	src/raw_logging.cc
	src/utilities.h
	src/utilities.cc
	src/vlog_is_on.cc
	src/config.h
	src/config_freebsd.h
	src/config_hurd.h
	src/config_linux.h
	src/config_mac.h
	src/base/commandlineflags.h
	src/base/googleinit.h
	src/base/mutex.h
	src/stacktrace.h
	src/stacktrace_generic-inl.h
	src/stacktrace_libunwind-inl.h
	src/stacktrace_powerpc-inl.h
	src/stacktrace_x86_64-inl.h
	src/stacktrace_x86-inl.h)

if(WIN32)
	list(APPEND SRC
		src/windows/port.cc
		src/windows/glog/raw_logging.h
		src/windows/glog/vlog_is_on.h
		src/windows/glog/logging.h
		src/windows/glog/log_severity.h
		src/windows/port.h
		src/windows/config.h)
	list(APPEND INC
		src/windows)
else()
	list(APPEND SRC
		src/demangle.cc
		src/signalhandler.cc
		src/symbolize.cc
		src/demangle.h
		src/glog/logging.h
		src/glog/log_severity.h
		src/glog/raw_logging.h
		src/glog/vlog_is_on.h
		src/symbolize.h)
endif()

add_library(${GLOG_LIBRARY} ${INC} ${SRC})
