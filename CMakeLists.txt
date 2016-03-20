cmake_minimum_required (VERSION 3.1.0)

project (zsoundproc)

list(APPEND CMAKE_MODULE_PATH "${zsoundproc_SOURCE_DIR}/cmake")

# Include libzmq

set(ZMQ_BUILD_TESTS OFF CACHE BOOL "Build the tests for ZeroMQ")
add_subdirectory(dependencies/libzmq)
include_directories(dependencies/libzmq/include)

set_target_properties(libzmq
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/bin"
)

if (LUAZMQ_USE_LIBSODIUM)
	add_dependencies(libzmq libsodium)
	target_compile_definitions(libzmq PRIVATE -DHAVE_LIBSODIUM)
endif()

# Include NSFFT

add_subdirectory(dependencies/nsfft-1.00)
include_directories(dependencies/nsfft-1.00/simd)
include_directories(dependencies/nsfft-1.00/dft)

# Include libsamplerate

add_subdirectory(dependencies/libsamplerate-0.1.8)
include_directories(dependencies/libsamplerate-0.1.8/src)

# Include libMODplug

add_subdirectory(dependencies/libmodplug-0.8.8.4)
include_directories(dependencies/libmodplug-0.8.8.4/include)

# Include libVorbis

add_subdirectory(dependencies/libvorbis-1.3.3)
include_directories(dependencies/libvorbis-1.3.3/include)

# Include libOGG

if (NOT TARGET OGG)
	add_subdirectory(dependencies/libogg-1.3.1)
endif()

include_directories(dependencies/libogg-1.3.1/include)

# Include libMAD

add_subdirectory(dependencies/libmad-0.15.1b)
include_directories(dependencies/libmad-0.15.1b)

# Include Lua 5.1

include(FindLua51)
include_directories(SYSTEM
	${LUA_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}/lua5.1
)

include_directories(dependencies/lutok2/include)

add_subdirectory(src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
set_target_properties(zsoundproc PROPERTIES PREFIX "")

