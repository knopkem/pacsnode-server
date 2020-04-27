CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(cuteLogger)

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#

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

ADD_DEFINITIONS(-DCUTELOGGER_LIBRARY)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(BEFORE include)

SET(sources
  src/Logger.cpp
  src/AbstractAppender.cpp
  src/AbstractStringAppender.cpp
  src/ConsoleAppender.cpp
  src/FileAppender.cpp
  src/RollingFileAppender.cpp
)

SET(includes
  include/Logger.h
  include/FileAppender.h
  include/ConsoleAppender.h
  include/AbstractStringAppender.h
  include/AbstractAppender.h
  include/RollingFileAppender.h
 )


# OutputDebugAppender is only for Windows systems
IF(WIN32)
  SET(sources ${sources} src/OutputDebugAppender.cpp)
  SET(includes ${includes} include/OutputDebugAppender.h)
ENDIF(WIN32)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${sources} ${includes})
qt5_use_modules(${PROJECT_NAME} Core)

include (GenerateExportHeader)
GENERATE_EXPORT_HEADER( ${PROJECT_NAME}  
             BASE_NAME ${PROJECT_NAME}
             EXPORT_MACRO_NAME ${PROJECT_NAME}_EXPORT
             EXPORT_FILE_NAME ${PROJECT_NAME}_Export.h
             STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)


#INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib)
