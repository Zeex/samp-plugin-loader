cmake_minimum_required(VERSION 2.8)
project(loader)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AMXConfig)
include(AddSAMPPlugin)

include_directories(sdk/amx)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_samp_plugin(loader
  configreader.cpp
  configreader.h
  plugin.cpp
  plugin.h
  loader.cpp
  loader.def
)

if(UNIX)
  find_library(LIBDL_PATH NAMES dl)
  if(LIBDL_PATH)
    target_link_libraries(loader ${LIBDL_PATH})
  endif()
endif()
