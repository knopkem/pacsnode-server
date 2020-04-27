project(tufao)

cmake_minimum_required(VERSION 2.8)

foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

# Metadata
set(TUFAO_VERSION_MAJOR 0)
set(TUFAO_VERSION_MINOR 8)
set(TUFAO_VERSION_PATCH 4)
set(TUFAO_VERSION ${TUFAO_VERSION_MAJOR}.${TUFAO_VERSION_MINOR}.${TUFAO_VERSION_PATCH})
set(TUFAO_BRIEF "An asynchronous web framework for C++ built on top of Qt")

# Dependencies
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(BUFFER_SIZE 128 CACHE STRING
    "The default buffer size (in bytes) used by Tufão")
add_definitions(-DBUFFER_SIZE=${BUFFER_SIZE})

# Build info
set(TUFAO_LIBRARY "tufao")

# Targets
add_subdirectory(src)

