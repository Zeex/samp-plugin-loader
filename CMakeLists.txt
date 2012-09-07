cmake_minimum_required(VERSION 2.8)

project(pluginloader)

set(SOURCES 
	configreader.cpp
	configreader.h
	plugin.cpp
	plugin.h
	pluginloader.cpp
)

if(WIN32)
	list(APPEND SOURCES pluginloader.def)
endif()

if(UNIX)
	add_definitions(-DLINUX)
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_library(pluginloader MODULE ${SOURCES})

if(UNIX)
	find_library(LIBDL_PATH NAMES dl)
	if(LIBDL_PATH)
		target_link_libraries(pluginloader ${LIBDL_PATH})
	endif()
endif()
