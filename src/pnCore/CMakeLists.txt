PROJECT(pnCore)

# applying global policies
foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

SET (HEADERS_WRAP
    sqlDatabase.h
    )

SET(HEADERS 
    ${HEADERS_WRAP}
    commonHelper.h
    dicomNode.h
    dicomNodeConfig.h
    pacsnode_global.h
    storage.h
   )

SET(SOURCES 
    commonHelper.cpp
    dicomNode.cpp
    dicomNodeConfig.cpp
    sqlDatabase.cpp
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

qt5_use_modules(${PROJECT_NAME} Core Gui Widgets Network Sql)

# Group common files together in Visual Studio.
SOURCE_GROUP("Generated Files" FILES ${HEADERS_MOC})

# Disable deprecation warnings 
ADD_DEFINITIONS(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_WARNINGS
)

