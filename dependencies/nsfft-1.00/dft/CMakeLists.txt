cmake_minimum_required (VERSION 3.1.0)
project (DFT)

CONFIGURE_FILE(DFTUndiff.c DFTpurecfloat.c COPYONLY)
CONFIGURE_FILE(DFTUndiff.c DFTpurecdouble.c COPYONLY)
CONFIGURE_FILE(DFTUndiff.c DFTpureclongdouble.c COPYONLY)
CONFIGURE_FILE(DFTUndiff.c DFTssefloat.c COPYONLY)
CONFIGURE_FILE(DFTUndiff.c DFTsse2double.c COPYONLY)
CONFIGURE_FILE(DFTUndiff.c DFTavxfloat.c COPYONLY)
CONFIGURE_FILE(DFTUndiff.c DFTavxdouble.c COPYONLY)

set(DFT_BASE_FLAGS "")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(DFT_src
	DFT.c
)

set(DFT_hdr
	DFT.h
	DFT.h
)

list(APPEND DFT_src
	DFTpurecfloat.c
	DFTpurecdouble.c
	DFTpureclongdouble.c
)

set_source_files_properties(DFTpurecfloat.c
	PROPERTIES
		COMPILE_FLAGS "-DENABLE_PUREC_FLOAT"
		LANGUAGE CXX
)
set_source_files_properties(DFTpurecdouble.c
	PROPERTIES
		COMPILE_FLAGS "-DENABLE_PUREC_DOUBLE"
		LANGUAGE CXX
)
set_source_files_properties(DFTpureclongdouble.c
	PROPERTIES
		COMPILE_FLAGS "-DENABLE_PUREC_LONGDOUBLE"
		LANGUAGE CXX
)


list(APPEND DFT_BASE_FLAGS
	ENABLE_PUREC_FLOAT
	ENABLE_PUREC_DOUBLE
	ENABLE_PUREC_LONGDOUBLE
)

if (DFT_SSE)
	list(APPEND DFT_BASE_FLAGS
		ENABLE_SSE_FLOAT
	)
	list(APPEND DFT_src
		DFTssefloat.c
	)
	set_source_files_properties(DFTssefloat.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_SSE_FLOAT"
			LANGUAGE CXX
	)
endif()

if (DFT_SSE2)
	list(APPEND DFT_BASE_FLAGS
		ENABLE_SSE2_DOUBLE
	)
	list(APPEND DFT_src
		DFTsse2double.c
	)
	set_source_files_properties(DFTsse2double.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_SSE2_DOUBLE"
			LANGUAGE CXX
	)
endif()

if (DFT_AVX)
	list(APPEND DFT_BASE_FLAGS
		ENABLE_AVX_FLOAT
		ENABLE_AVX_DOUBLE
	)

	list(APPEND DFT_src
		DFTavxfloat.c
		DFTavxdouble.c
	)
	set_source_files_properties(DFTavxfloat.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_AVX_FLOAT"
			LANGUAGE CXX
	)
	set_source_files_properties(DFTavxdouble.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_AVX_DOUBLE"
			LANGUAGE CXX
	)
endif()

set_source_files_properties(DFT.c
	PROPERTIES
		COMPILE_DEFINITIONS "${DFT_BASE_FLAGS}"
		LANGUAGE CXX
)

set(DFT_sources
	${DFT_src} ${DFT_hdr}
)

include_directories(./)

add_library(DFT STATIC
	${DFT_sources}
)

set_target_properties(DFT
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/bin"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
set_target_properties(DFT PROPERTIES PREFIX "")
