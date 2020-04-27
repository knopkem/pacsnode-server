PROJECT(pacsnode)
cmake_minimum_required(VERSION 2.8)

set(project_policies
  CMP0001 # NEW: CMAKE_BACKWARDS_COMPATIBILITY should no longer be used.
  CMP0002 # NEW: Logical target names must be globally unique.
  CMP0003 # NEW: Libraries linked via full path no longer produce linker search paths.
  CMP0004 # NEW: Libraries linked may NOT have leading or trailing whitespace.
  CMP0005 # NEW: Preprocessor definition values are now escaped automatically.
  CMP0006 # NEW: Installing MACOSX_BUNDLE targets requires a BUNDLE DESTINATION.
  CMP0007 # NEW: List command no longer ignores empty elements.
  CMP0008 # NEW: Libraries linked by full-path must have a valid library file name.
  CMP0009 # NEW: FILE GLOB_RECURSE calls should not follow symlinks by default.
  CMP0010 # NEW: Bad variable reference syntax is an error.
  CMP0011 # NEW: Included scripts do automatic cmake_policy PUSH and POP.
  CMP0012 # NEW: if() recognizes numbers and boolean constants.
  CMP0013 # NEW: Duplicate binary directories are not allowed.
  CMP0014 # NEW: Input directories must have CMakeLists.txt
  CMP0020 # NEW: disable autolinking to qtmain as we have our own main() functions (new in Qt 5.1)
  CMP0043 # NEW: Ignore COMPILE_DEFINITIONS_<Config> properties
  )
  
foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()


## #################################################################
## PROJECT SETTINGS
## #################################################################

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PN_VERSION_MAJOR 1 CACHE STRING "${PROJECT_NAME} major version number.")
set(PN_VERSION_MINOR 0 CACHE STRING "${PROJECT_NAME} minor version number.")
set(PN_VERSION_BUILD 0 CACHE STRING "${PROJECT_NAME} build version number.")


configure_file( cmake/version_config.h.in ${CMAKE_BINARY_DIR}/generated/version_config.h )
include_directories( ${CMAKE_BINARY_DIR}/generated/ ) # Make sure it can be included

set(BIN_INSTALL_DIR ".")

IF(WIN32)
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install_dir CACHE INTERNAL "Prefix prepended to install directories" FORCE)
ENDIF()

#-----------------------------------------------------------------------------
# Set a default build type if none was specified
#-----------------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebugInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
               STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# determine deployment system
if(WIN32)
  option(SHOW_CONSOLE "Show console at runtime (mainly for debugging)" "FALSE")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
ENDIF(WIN32)
set(DEPLOYMENT_SYSTEM)
IF(APPLE)
  set(DEPLOYMENT_SYSTEM MACOSX_BUNDLE)
endif()
if(WIN32)
  if( NOT ${SHOW_CONSOLE} )
    set(DEPLOYMENT_SYSTEM WIN32)
  endif() 
endif()

if(UNIX OR APPLE)
    SET(CMAKE_CXX_FLAGS "-std=c++11")
endif()

## #################################################################
## DEPENDENCIES
## #################################################################

# Qt
INCLUDE(FindQt5)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)

# DCMTK
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIR})

 
## #################################################################
## SUB PROJECTS
## #################################################################

add_subdirectory(libs)
add_subdirectory(src)



## #################################################################
## DOCUMENTATION
## #################################################################

FIND_PACKAGE(Doxygen)
IF (DOXYGEN)
  SET(DOXYGEN_BUILD_DIR ${PROJECT_BINARY_DIR}/Doxygen)

  CONFIGURE_FILE (
    ${PROJECT_SOURCE_DIR}/cmake/doxygen.config.in
    ${DOXYGEN_BUILD_DIR}/doxygen.config
  ) 

  ADD_CUSTOM_TARGET(${PROJECT_NAME}-doc
    ${DOXYGEN}
    ${DOXYGEN_BUILD_DIR}/doxygen.config
    )
ENDIF (DOXYGEN)

## #################################################################
## PACKAGING
## #################################################################
 
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pacsnode Installer")
SET(CPACK_PACKAGE_VENDOR "Michael Knopke")
SET(CPACK_PACKAGE_CONTACT "Michael Knopke knopkem@pacsnode.com")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Description.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Welcome.txt") #use Welcome instead of copyrights for the moment
#SET(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/Welcome.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NON-PUPLIC PROTOTYPE VERSION INTERNAL USE ONLY!")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PN_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PN_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PN_VERSION_BUILD})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}") 
SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
 
# Installers for 32- vs. 64-bit CMake: 
#  - Root install directory (displayed to end user at installer-run time) 
#  - "NSIS package/display name" (text used in the installer GUI) 
#  - Registry key used to store info about the installation 
  IF(CMAKE_CL_64) 
    SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64") 
    SET(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} (Win64)") 
    SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} 
${CPACK_PACKAGE_VERSION} (Win64)") 
  ELSE()
    SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES") 
    SET(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}") 
    SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} 
${CPACK_PACKAGE_VERSION}") 
  ENDIF() 

if(WIN32)
    SET(CPACK_GENERATOR "NSIS")
    SET(CPACK_NSIS_DISPLAY_NAME "Pacsnode")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\www.pacsnode.com")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\www.pacsnode.com")
    SET(CPACK_NSIS_CONTACT "knopkem@pacsnode.com")
    SET(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/src/pacsnode/resources/pacsnode.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/src/pacsnode/resources/pacsnode.ico")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}")
    set(CPACK_CREATE_DESKTOP_LINKS "pacsnode")
endif()
INCLUDE(CPack)

## #################################################################
## INSTALL
## #################################################################

#deploy qt plugins
add_subdirectory(install)
INSTALL( DIRECTORY ${QT5_ROOT_PREFIX}/plugins/platforms DESTINATION ${BIN_INSTALL_DIR} )
INSTALL( DIRECTORY ${QT5_ROOT_PREFIX}/plugins/imageformats DESTINATION ${BIN_INSTALL_DIR} )
INSTALL( DIRECTORY ${QT5_ROOT_PREFIX}/plugins/sqldrivers DESTINATION ${BIN_INSTALL_DIR} )
