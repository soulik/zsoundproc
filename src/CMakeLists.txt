set(luazsoundproc_src
	main.cpp
)

set(luazsoundproc_h
	common.hpp
)

set(luazsoundproc_sources
	${luazsoundproc_src} ${luazsoundproc_h}
)

include_directories(./)

source_group("Header files" FILES ${luazsoundproc_h})

set(luazsoundproc_version_script luazsoundproc.version)

IF(UNIX)
    IF(CMAKE_COMPILER_IS_GNUCC)
         SET(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_CXX_COMPILE_OPTIONS_VISIBILITY}hidden -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/${luazsoundproc_version_script}")
    ENDIF(CMAKE_COMPILER_IS_GNUCC)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations")
ENDIF(UNIX)

add_library(luazsoundproc SHARED
	${luazsoundproc_sources}
)

set_property(TARGET luazsoundproc PROPERTY CXX_STANDARD 11)
set_property(TARGET luazsoundproc PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(luazsoundproc
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/bin"
)

configure_file(zsoundproc.lua ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib/zsoundproc.lua COPYONLY)

target_link_libraries (luazsoundproc
	${LUA_LIBRARY}
)

