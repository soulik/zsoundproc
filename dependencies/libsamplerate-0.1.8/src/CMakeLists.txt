cmake_minimum_required (VERSION 3.1.0)

set(PACKAGE "libsamplerate")
set(PACKAGE_BUGREPORT "erikd@mega-nerd.com")
set(PACKAGE_NAME "libsamplerate")
set(PACKAGE_STRING "libsamplerate-0.1.8")
set(PACKAGE_TARNAME "")
set(PACKAGE_URL "http://www.mega-nerd.com/SRC/index.html")
set(PACKAGE_VERSION "0.1.8")
set(VERSION "0.1.8")

include(TestBigEndian)

TEST_BIG_ENDIAN(BIGENDIAN)

if(BIGENDIAN)
	set(CPU_IS_BIG_ENDIAN 1)
else()
	set(CPU_IS_LITTLE_ENDIAN 1)
endif()

set(CPU_CLIPS_NEGATIVE "0" CACHE STRING "Target processor clips on negative float to int conversion.")
set(CPU_CLIPS_POSITIVE "0" CACHE STRING "Target processor clips on positive float to int conversion.")

if(WIN32)
	set(OS_IS_WIN32 1)
endif()

if(MSVC)
	set(COMPILER_IS_GCC "0")
else()
	set(COMPILER_IS_GCC "1")
endif()

set(LT_OBJDIR ".libs/")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(libsamplerate_src
	samplerate.c
	src_linear.c
	src_sinc.c
	src_zoh.c
)

set(libsamplerate_hdr
	common.h
	config.h.in
	fastest_coeffs.h
	float_cast.h
	high_qual_coeffs.h
	mid_qual_coeffs.h
	samplerate.h
)

set(libsamplerate_sources
	${libsamplerate_src} ${libsamplerate_hdr}
)

configure_file("${libsamplerate_SOURCE_DIR}/src/config.h.cmake"
  "${libsamplerate_BINARY_DIR}/src/config.h")
configure_file("${libsamplerate_SOURCE_DIR}/Win32/unistd.h"
  "${libsamplerate_BINARY_DIR}/src/unistd.h")
configure_file("${libsamplerate_SOURCE_DIR}/Win32/libsamplerate-0.def" "${libsamplerate_BINARY_DIR}/libsamplerate-0.def" @ONLY)


include_directories(./)
include_directories("${libsamplerate_BINARY_DIR}/src")

add_library(libsamplerate SHARED
	${libsamplerate_sources}
)

if(WIN32)
	if(MSVC)
		set_target_properties(libsamplerate PROPERTIES LINK_FLAGS "/DEF:${libsamplerate_BINARY_DIR}/libsamplerate-0.def")
	endif()
endif()

set_target_properties(libsamplerate
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/bin"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
set_target_properties(DFT PROPERTIES PREFIX "")
