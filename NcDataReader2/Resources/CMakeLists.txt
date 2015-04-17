cmake_minimum_required(VERSION 2.6)

project(ncDataReader2)

set(NCDR_VERSION_MAJOR "2")
set(NCDR_VERSION_MINOR "3")
set(NCDR_VERSION_PATCH "1")
set(NCDR_VERSION "${NCDR_VERSION_MAJOR}.${NCDR_VERSION_MINOR}.${NCDR_VERSION_PATCH}")
set(NCDR_PACKAGE_NAME "ncDataReader2")
set(NCDR_PACKAGE_FULLNAME "${NCDR_PACKAGE_NAME}-${NCDR_VERSION}")
set(NCDR_MAINTAINER "Joerg Raedler <joerg@j-raedler.de>")

option(BUILD_SHARED_LIB "build a shared library" OFF)

option(MODELICA_ERROR_HANDLER "enable ModelicaError functions" ON)

option(NC_NOCHECK "disable error handling functions (not recommended!)" OFF)

if(WIN32)
  option(WIN32_MSGBOX "show errors with a MessageBox on windows" ON)
endif(WIN32)

INCLUDE(InstallRequiredSystemLibraries)

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

if(MODELICA_ERROR_HANDLER)
  add_definitions(-DMODELICA_ERROR_HANDLER)
endif(MODELICA_ERROR_HANDLER)

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

include_directories("${PROJECT_BINARY_DIR}/src" "${PROJECT_BINARY_DIR}" "${NC_INC}" "${PROJECT_SOURCE_DIR}/src")

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(examples)
add_subdirectory(doc)

# SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My funky project")
SET(CPACK_PACKAGE_VENDOR "${NCDR_MAINTAINER}")
SET(CPACK_PACKAGE_CONTACT "${NCDR_MAINTAINER}")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../README.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "${NCDR_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${NCDR_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${NCDR_VERSION_PATCH}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE")

IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  # SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  # SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  # SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
  # SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
  # SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
  # SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  # SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  # SET(CPACK_STRIP_FILES "bin/MyExecutable")
  # SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_GENERATOR "DEB")
INCLUDE(CPack)
