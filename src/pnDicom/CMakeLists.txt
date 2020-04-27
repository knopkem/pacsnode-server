project(pnDicom)

# applying global policies
foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

if(WIN32)
  add_definitions (-DNOMINMAX)
endif()

add_definitions(-DHAVE_CONFIG_H)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src/pnCore
    ${CMAKE_SOURCE_DIR}/src/pnCore
    ${CMAKE_BINARY_DIR}/libs/libiconv
    ${CMAKE_SOURCE_DIR}/libs/libiconv
    ${CMAKE_BINARY_DIR}/libs/dcmtkJ2K
    ${CMAKE_SOURCE_DIR}/libs/dcmtkJ2K    
    )
    
set(HEADERS_WRAP
    dicomServer.h
    )
    
set(HEADERS
    ${HEADERS_WRAP}
    dicomElement.h
    dicomHelper.h
    findScu.h
    iFindScu.h
    iMoveScu.h
    moveScu.h
    multiFinder.h
    scpImpl.h
    tableCreator.h
    tableDefinition.h
    tableElement.h
    tableFinder.h
    tableUpdater.h    
    )

set(SOURCES
    dicomElement.cpp
    dicomHelper.cpp
    dicomServer.cpp
    findScu.cpp
    moveScu.cpp
    multiFinder.cpp
    scpImpl.cpp
    tableCreator.cpp
    tableFinder.cpp
    tableUpdater.cpp
    )

QT5_WRAP_CPP(HEADERS_MOC ${HEADERS_WRAP})
QT5_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})


add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
  ${SOURCES} 
  ${HEADERS_MOC}
  ${HEADERS}
  ${RESOURCES_QRC}
  )
  
include (GenerateExportHeader)
GENERATE_EXPORT_HEADER( ${PROJECT_NAME}  
             BASE_NAME ${PROJECT_NAME}
             EXPORT_MACRO_NAME ${PROJECT_NAME}_EXPORT
             EXPORT_FILE_NAME ${PROJECT_NAME}_Export.h
             STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

target_link_libraries(${PROJECT_NAME}
  ${DCMTK_LIBRARIES}
  pnCore
)

qt5_use_modules(${PROJECT_NAME} Core Gui Sql Concurrent)

# Group common files together in Visual Studio.
SOURCE_GROUP("Generated Files" FILES ${HEADERS_MOC})
