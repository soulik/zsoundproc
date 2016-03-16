cmake_minimum_required (VERSION 3.1.0)
project (NSFFT)

set(DFT_SSE OFF CACHE BOOL "Support SSE instructions")
set(DFT_SSE2 OFF CACHE BOOL "Support SSE2 instructions")
set(DFT_AVX OFF CACHE BOOL "Support AVX instructions")

# Include SIMD

add_subdirectory(simd)
include_directories(simd)

# Include DFT

add_subdirectory(dft)
include_directories(dft)

add_dependencies(DFT SIMD)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
#set_target_properties(NSFFT PROPERTIES PREFIX "")

