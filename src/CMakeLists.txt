set(zsoundproc_src
	main.cpp
	constants.cpp
)

set(zsoundproc_h
	common.hpp
	constants.hpp
	init_classes.hpp
)

set(zsoundproc_objects_src
	objects/Processor.cpp
)

set(zsoundproc_objects_hdr
	objects/Processor.hpp
)

set(zsoundproc_sources
	${zsoundproc_src} ${zsoundproc_h}
	${zsoundproc_objects_src} ${zsoundproc_objects_hdr}
)

source_group("Header files" FILES ${zsoundproc_h})
source_group(objects FILES ${zsoundproc_objects_src} ${zsoundproc_objects_hdr})

include_directories(./)

source_group("Header files" FILES ${zsoundproc_h})

set(zsoundproc_version_script zsoundproc.version)

IF(UNIX)
    IF(CMAKE_COMPILER_IS_GNUCC)
         SET(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_CXX_COMPILE_OPTIONS_VISIBILITY}hidden -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/${zsoundproc_version_script}")
    ENDIF(CMAKE_COMPILER_IS_GNUCC)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations")
ENDIF(UNIX)

add_library(zsoundproc SHARED
	${zsoundproc_sources}
)

set_property(TARGET zsoundproc PROPERTY CXX_STANDARD 11)
set_property(TARGET zsoundproc PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(zsoundproc
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/bin"
)

configure_file(zsoundproc.lua ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/lib/zsoundproc.lua COPYONLY)

target_link_libraries (zsoundproc
	${LUA_LIBRARY}
)

