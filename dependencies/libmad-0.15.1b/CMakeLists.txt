cmake_minimum_required (VERSION 3.1.0)

project (LIBMAD)

list(APPEND CMAKE_MODULE_PATH "${LIBMAD_SOURCE_DIR}/cmake")
option(LIBMAD_AMD64 "Enable building amd64 assembly implementation")

include(CheckLibraryExists)
include(CheckIncludeFiles)
include(CheckIncludeFile)
include(AC_HEADER_STDC)
include(TestBigEndian)

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/)
include_directories(BEFORE ./)

set(LIBMAD_SRC
	bit.c
	decoder.c
	fixed.c
	frame.c
	huffman.c
	layer12.c
	layer3.c
	stream.c
	synth.c
	timer.c
	version.c
)

set(LIBMAD_H
	bit.h
	decoder.h
	fixed.h
	frame.h
	global.h
	huffman.h
	layer12.h
	layer3.h
	mad.h
	stream.h
	synth.h
	timer.h
	version.h
)

set(LIBMAD_DATA
	D.dat
	imdct_s.dat
	qc_table.dat
	rq_table.dat
	sf_table.dat
)

set(LIBMAD_SOURCES
	${LIBMAD_H}
	${LIBMAD_SRC}
	${LIBMAD_DATA}
)

set(LIBMAD_CONTROLS OFF CACHE BOOL "Embeed media player controls")

if(NOT LIBMAD_CONTROLS)
	add_definitions(-DNOCONTROLS)
endif()

include_directories("${LIBMAD_SOURCE_DIR}")

add_library(LIBMAD STATIC
	${LIBMAD_SOURCES}
)

TEST_BIG_ENDIAN(BIGENDIAN)

if(BIGENDIAN)
	set(WORDS_BIGENDIAN 1)
endif()

check_symbol_exists(pid_t sys/types.h pid_t_DEFINED)
if (NOT pid_t_DEFINED)
	set(pid_t int)
endif()

if (("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64|AMD64"))
	add_definitions(-DFPM_64BIT)
elseif (("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86"))
	add_definitions(-DFPM_INTEL)
elseif (("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm"))
	add_definitions(-DFPM_ARM)
else()
	message("Unknown architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

add_definitions(-DHAVE_CONFIG_H)

if(WIN32)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/msvc++/config.h ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/msvc++/mad.h ${CMAKE_CURRENT_BINARY_DIR}/mad.h @ONLY)
else()
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mad.h ${CMAKE_CURRENT_BINARY_DIR}/mad.h @ONLY)
endif()

set_target_properties(LIBMAD
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(WIN32)
	if(MSVC)
		#add_definitions(-DDECLSPEC="_declspec(dllexport)")
	endif()
endif()

target_link_libraries (LIBMAD
	${SDL2_LIBRARY}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
set_target_properties(LIBMAD
	PROPERTIES PREFIX "" OUTPUT_NAME "libmad"
)
