cmake_minimum_required(VERSION 2.8)

project(loader)

include_directories(sdk/amx)

if(WIN32)
  if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  endif()
endif()

if(UNIX)
  add_definitions(-DLINUX)
endif()

set(SOURCES 
  configreader.cpp
  configreader.h
  plugin.cpp
  plugin.h
  loader.cpp
)

if(WIN32)
  list(APPEND SOURCES loader.def)
endif()

add_library(loader MODULE ${SOURCES})
set_target_properties(loader PROPERTIES PREFIX "")

if(UNIX)
  find_library(LIBDL_PATH NAMES dl)
  if(LIBDL_PATH)
    target_link_libraries(loader ${LIBDL_PATH})
  endif()
endif()
