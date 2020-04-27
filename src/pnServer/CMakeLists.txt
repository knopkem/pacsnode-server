PROJECT(pnServer)

foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

if(WIN32)
  add_definitions (-DNOMINMAX)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src/pnCore
    ${CMAKE_SOURCE_DIR}/src/pnCore
    ${CMAKE_BINARY_DIR}/src/pnDicom
    ${CMAKE_SOURCE_DIR}/src/pnDicom
    ${CMAKE_BINARY_DIR}/src/pnHttp
    ${CMAKE_SOURCE_DIR}/src/pnHttp
    ${CMAKE_BINARY_DIR}/libs/libiconv
    ${CMAKE_SOURCE_DIR}/libs/libiconv
    ${CMAKE_BINARY_DIR}/libs/dcmtkJ2K
    ${CMAKE_SOURCE_DIR}/libs/dcmtkJ2K
    ${CMAKE_SOURCE_DIR}/libs/tufao/src
    ${CMAKE_SOURCE_DIR}/libs/tufao/include
    ${CMAKE_SOURCE_DIR}/libs/tufao/src
    ${CMAKE_BINARY_DIR}/libs/cuteLogger
    ${CMAKE_SOURCE_DIR}/libs/cuteLogger/include
    ${CMAKE_SOURCE_DIR}/libs/cuteLogger/src   
    )
    
set(HEADERS_WRAP
    eventFilter.h
    httpServerStarter.h
    sigHandler.h
    mainServer.h
    moveRequestor.h    
    queryHandler.h
    workDispatcher.h
    )
    
set(HEADERS
    ${HEADERS_WRAP}
    imageCropper.h
    imageWorker.h    
    patientData.h
    util.h
    )

set(SOURCES
    httpServerStarter.cpp
    eventFilter.cpp
    imageWorker.cpp
    main.cpp
    mainServer.cpp
    moveRequestor.cpp    
    patientData.cpp
    queryHandler.cpp
    sigHandler.cpp
    util.cpp
    workDispatcher.cpp
    )

QT5_WRAP_CPP(HEADERS_MOC ${HEADERS_WRAP})
QT5_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})

add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS_MOC}
    ${HEADERS}
    )
    
target_link_libraries(${PROJECT_NAME} 
    ${DCMTK_LIBRARIES}
    tufao
    pnCore
    pnDicom
    cuteLogger
    )

qt5_use_modules(${PROJECT_NAME} Core Gui Sql Network Concurrent)

# Group common files together in Visual Studio.
SOURCE_GROUP("Generated Files" FILES ${HEADERS_MOC})

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION  ${BIN_INSTALL_DIR} BUNDLE DESTINATION  ${BIN_INSTALL_DIR}  )

