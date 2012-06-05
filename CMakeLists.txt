cmake_minimum_required(VERSION 2.6)
project(ncDataReader2)

set(NCDR_VERSION "2.3.0")

option(BUILD_SHARED_LIB "build a shared library" OFF)

option(NC_NOCHECK "disable error handling functions (not recommended!)" OFF)

if(WIN32)
  option(WIN32_MSGBOX "show errors with a MessageBox on windows" ON)
endif(WIN32)

if(BUILD_SHARED_LIB)
  set(LIB_BUILD_TYPE "SHARED")
  add_definitions(-DBUILD_SHARED)
  if(WIN32)
    set(DLL_EXPORT "__declspec(dllexport)")
  else(WIN32)
    set(DLL_EXPORT "")
  endif(WIN32)
else(BUILD_SHARED_LIB)
  set(LIB_BUILD_TYPE "STATIC")
endif(BUILD_SHARED_LIB)

if(NC_NOCHECK)
  add_definitions(-DNC_NOCHECK)
endif(NC_NOCHECK)

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  if(WIN32_MSGBOX)
    add_definitions(-DWIN32_MSGBOX)
  endif(WIN32_MSGBOX)
endif(WIN32)

configure_file("config.h.in" "config.h")

find_path(NC_INC netcdf.h)
find_library(NC_LIB NAMES netcdf)

if(UNIX)
   set(MATH_LIB m)
else(UNIX)
   set(MATH_LIB)
endif(UNIX)

include_directories("${NC_INC}" "${PROJECT_SOURCE_DIR}/src")

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(doc)
