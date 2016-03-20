cmake_minimum_required (VERSION 3.1.0)

project (libsamplerate)

list(APPEND CMAKE_MODULE_PATH "${libsamplerate_SOURCE_DIR}/cmake")

include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckIncludeFiles)
include(CheckIncludeFile)
include(AC_HEADER_STDC)

add_subdirectory(src)
include_directories(src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
#set_target_properties(NSFFT PROPERTIES PREFIX "")

