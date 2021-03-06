cmake_minimum_required (VERSION 3.1.0)
# enable_language(ASM-NASM)

project (FLAC)

include(CheckLibraryExists)
include(CheckIncludeFiles)
include(CheckIncludeFile)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/nasm.cmake)

# Ogg Vorbis

if (NOT TARGET OGG)
	add_subdirectory(../libogg-1.3.1 "${CMAKE_CURRENT_BINARY_DIR}/libogg")
endif()
include_directories(../libogg-1.3.1/include)

add_definitions(-DVERSION="1.3.1" -DFLAC_API_EXPORTS)

set(FLAC_CPU_X86_64 OFF CACHE BOOL "Use 64-bit architecture")

if (NOT FLAC_CPU_X86_64)
	set(FLAC_USE_ASM OFF CACHE BOOL "Use assembler")
endif()

set(FLAC_USE_3DNOW OFF CACHE BOOL "Use 3DNow!")
set(FLAC_USE_AVX OFF CACHE BOOL "Use AVX instructions")
set(FLAC_USE_SSE OFF CACHE BOOL "Use SSE instructions")
set(FLAC_USE_SSE2 OFF CACHE BOOL "Use SSE 2 instructions")
set(FLAC_USE_SSE41 OFF CACHE BOOL "Use SSE 4.1 instructions")

set(FLAC_SRC_FLAC
	src/flac/analyze.c
	src/flac/decode.c
	src/flac/encode.c
	src/flac/foreign_metadata.c
	src/flac/iffscan.c
	src/flac/local_string_utils.c
	src/flac/main.c
	src/flac/utils.c
	src/flac/vorbiscomment.c
)

set(FLAC_SRC_LIBFLAC
	src/libflac/bitmath.c
	src/libflac/bitreader.c
	src/libflac/bitwriter.c
	src/libflac/cpu.c
	src/libflac/crc.c
	src/libflac/fixed.c
	src/libflac/float.c
	src/libflac/format.c
	src/libflac/lpc.c
	src/libflac/md5.c
	src/libflac/memory.c
	src/libflac/metadata_iterators.c
	src/libflac/metadata_object.c
	src/libflac/ogg_decoder_aspect.c
	src/libflac/ogg_encoder_aspect.c
	src/libflac/ogg_helper.c
	src/libflac/ogg_mapping.c
	src/libflac/stream_decoder.c
	src/libflac/stream_encoder.c
	src/libflac/stream_encoder_framing.c
	src/libflac/window.c
)

set(FLAC_SRC_SHARE
)

if (FLAC_USE_AVX)
	list(APPEND FLAC_SRC_LIBFLAC
		src/libflac/lpc_intrin_avx2.c
		src/libflac/stream_encoder_intrin_avx2.c
	)
endif()

if (FLAC_USE_SSE)
	list(APPEND FLAC_SRC_LIBFLAC
		src/libflac/lpc_intrin_sse.c
	)
endif()

if (FLAC_USE_SSE2)
	list(APPEND FLAC_SRC_LIBFLAC
		src/libflac/lpc_intrin_sse2.c
		src/libflac/stream_encoder_intrin_sse2.c
	)
endif()

if (FLAC_USE_SSE41)
	list(APPEND FLAC_SRC_LIBFLAC
		src/libflac/lpc_intrin_sse41.c
		src/libflac/stream_encoder_intrin_ssse3.c
	)
endif()


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	add_definitions(-DFLAC__CPU_PPC -DFLAC__USE_ALTIVEC)
	set(FLAC_SRC_LIBFLAC_NASM
		src/libflac/ppc/as/lpc_asm.s
	)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Solaris")
	add_definitions(-DFLAC__NO_ASM)
ELSE()
	add_definitions(-DFLAC__HAS_X86INTRIN)

	if (FLAC_USE_ASM)
		add_definitions(-DFLAC__HAS_NASM)

		if (FLAC_USE_3DNOW)
			add_definitions(-DFLAC__USE_3DNOW)
		endif()
		
		if (FLAC_CPU_X86_64)
			add_definitions(-DFLAC__CPU_X86_64)
		else()
			add_definitions(-DFLAC__CPU_IA32)
			if (FLAC_USE_SSE OR FLAC_USE_SSE2 OR FLAC_USE_SSE41)
				add_definictions(-DFLAC__SSE_OS)
			endif()

			set(FLAC_SRC_LIBFLAC_NASM
				src/libflac/ia32/cpu_asm.nasm
				src/libflac/ia32/fixed_asm.nasm
				src/libflac/ia32/lpc_asm.nasm
			)
		endif()
	else()
		add_definitions(-DFLAC__NO_ASM)
	endif()
ENDIF()
add_definitions(-DFLAC__ALIGN_MALLOC_DATA)

if (FLAC_USE_ASM)
	nasm_command(FLAC_SRC_LIBFLAC_NASM FLAC_ASM_OUT)
endif()

if(WIN32)
	list(APPEND FLAC_SRC_SHARE
		src/share/win_utf8_io/win_utf8_io.c
	)
endif()

set(FLAC_SOURCES ${FLAC_SRC_LIBFLAC} ${FLAC_SRC_SHARE})

include_directories("${FLAC_SOURCE_DIR}/include")
include_directories("${FLAC_SOURCE_DIR}/src/libflac/include")

add_library(FLAC SHARED
	${FLAC_SOURCES}
	${FLAC_ASM_OUT}
)

if(WIN32)
#	set_property(TARGET FLAC APPEND_STRING PROPERTY
#		COMPILE_FLAGS "/FI\"${FLAC_SOURCE_DIR}/src/win32_dll.h\""
#	)
endif()

target_link_libraries (FLAC
	OGG
)

set_target_properties(FLAC
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
set_target_properties(FLAC
	PROPERTIES PREFIX "" OUTPUT_NAME "libFLAC"
)
