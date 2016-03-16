cmake_minimum_required (VERSION 3.1.0)
project (SIMD)

CONFIGURE_FILE(SIMDBaseUndiff.c SIMDBaseUndiff_purecfloat.c COPYONLY)
CONFIGURE_FILE(SIMDBaseUndiff.c SIMDBaseUndiff_purecdouble.c COPYONLY)
CONFIGURE_FILE(SIMDBaseUndiff.c SIMDBaseUndiff_pureclongdouble.c COPYONLY)
CONFIGURE_FILE(SIMDBaseUndiff.c SIMDBaseUndiff_ssefloat.c COPYONLY)
CONFIGURE_FILE(SIMDBaseUndiff.c SIMDBaseUndiff_sse2double.c COPYONLY)
CONFIGURE_FILE(SIMDBaseUndiff.c SIMDBaseUndiff_avxfloat.c COPYONLY)
CONFIGURE_FILE(SIMDBaseUndiff.c SIMDBaseUndiff_avxdouble.c COPYONLY)

set(SIMD_BASE_FLAGS "")

set(CXX_FLAG_SSE "/arch:SSE")
set(CXX_FLAG_SSE2 "/arch:SSE2")
set(CXX_FLAG_AVX "/arch:AVX")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(SIMD_src
	SIMDBase.c
)

set(SIMD_hdr
	SIMDBase.h
	SIMDBaseUndiff.h
)


list(APPEND SIMD_src
	SIMDBaseUndiff_purecfloat.c
	SIMDBaseUndiff_purecdouble.c
	SIMDBaseUndiff_pureclongdouble.c
)

set_source_files_properties(SIMDBaseUndiff_purecfloat.c
	PROPERTIES
		COMPILE_FLAGS "-DENABLE_PUREC_FLOAT"
		LANGUAGE CXX
)
set_source_files_properties(SIMDBaseUndiff_purecdouble.c
	PROPERTIES
		COMPILE_FLAGS "-DENABLE_PUREC_DOUBLE"
		LANGUAGE CXX
)
set_source_files_properties(SIMDBaseUndiff_pureclongdouble.c
	PROPERTIES
		COMPILE_FLAGS "-DENABLE_PUREC_LONGDOUBLE"
		LANGUAGE CXX
)

list(APPEND SIMD_BASE_FLAGS
	ENABLE_PUREC_FLOAT
	ENABLE_PUREC_DOUBLE
	ENABLE_PUREC_LONGDOUBLE
)

if (DFT_SSE)
	list(APPEND SIMD_BASE_FLAGS
		ENABLE_SSE_FLOAT
	)
	list(APPEND SIMD_src
		SIMDBaseUndiff_ssefloat.c
	)
	set_source_files_properties(SIMDBaseUndiff_ssefloat.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_SSE_FLOAT ${CXX_FLAG_SSE}"
			LANGUAGE CXX
	)
endif()

if (DFT_SSE2)
	list(APPEND SIMD_BASE_FLAGS
		ENABLE_SSE2_DOUBLE
	)
	list(APPEND SIMD_src
		SIMDBaseUndiff_sse2double.c
	)
	set_source_files_properties(SIMDBaseUndiff_sse2double.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_SSE2_DOUBLE ${CXX_FLAG_SSE2}"
			LANGUAGE CXX
	)
endif()

if (DFT_AVX)
	list(APPEND SIMD_BASE_FLAGS
		ENABLE_AVX_FLOAT
		ENABLE_AVX_DOUBLE
	)
	list(APPEND SIMD_src
		SIMDBaseUndiff_avxfloat.c
		SIMDBaseUndiff_avxdouble.c
	)
	set_source_files_properties(SIMDBaseUndiff_avxfloat.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_AVX_FLOAT ${CXX_FLAG_AVX}"
			LANGUAGE CXX
	)
	set_source_files_properties(SIMDBaseUndiff_avxdouble.c
		PROPERTIES
			COMPILE_FLAGS "-DENABLE_AVX_DOUBLE ${CXX_FLAG_AVX}"
			LANGUAGE CXX
	)
endif()

set_source_files_properties(SIMDBase.c
	PROPERTIES
		COMPILE_DEFINITIONS "${SIMD_BASE_FLAGS}"
		LANGUAGE CXX
)

set(SIMD_sources
	${SIMD_src} ${SIMD_hdr}
)

include_directories(./)

add_library(SIMD STATIC
	${SIMD_sources}
)

set_target_properties(SIMD
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/bin"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
set_target_properties(SIMD PROPERTIES PREFIX "")
