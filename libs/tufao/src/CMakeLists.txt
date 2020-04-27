
# Source files
set(tufao_SRC
    abstracthttpserverrequesthandler.cpp
    httpserverrequest.cpp
    httpserverresponse.cpp
    url.cpp
    querystring.cpp
    priv/reasonphrase.cpp
    priv/http_parser.c
    httpfileserver.cpp
    headers.cpp
    priv/rfc1123.cpp
    priv/rfc1036.cpp
    priv/asctime.cpp
    sessionstore.cpp
    simplesessionstore.cpp
    worker.cpp
    httpserver.cpp
    notfoundhandler.cpp
)

# MOC header files
set(tufao_MOC_HDRS
    abstracthttpserverrequesthandler.h
    httpserverrequest.h
    httpserverresponse.h
    httpfileserver.h
    sessionstore.h
    simplesessionstore.h
    worker.h
    httpserver.h
    notfoundhandler.h
)

qt5_wrap_cpp(tufao_MOC ${tufao_MOC_HDRS}
    OPTIONS
    "-DQT_VERSION_MAJOR=${Qt5Core_VERSION_MAJOR}"
    "-DQT_VERSION_MINOR=${Qt5Core_VERSION_MINOR}"
    "-DQT_VERSION_PATCH=${Qt5Core_VERSION_PATCH}"
)

# Group common files together in Visual Studio.
SOURCE_GROUP("Generated Files" FILES ${tufao_MOC})

add_library("${TUFAO_LIBRARY}" STATIC ${tufao_SRC} ${tufao_MOC})
qt5_use_modules("${TUFAO_LIBRARY}" Core Network)
