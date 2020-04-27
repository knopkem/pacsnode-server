#include "httpServerStarter.h"

#include "queryHandler.h"
#include "sqlDatabase.h"
#include "dicomNodeConfig.h"

#include <QtCore>

// tufao
#include <HttpServer>

namespace pacsnode {

class HttpServerPrivate {
public:
    Tufao::HttpServer* threadedServer;
    pacsnode::QueryHandler* queryHandler;
};

HttpServerStarter::HttpServerStarter(QObject* parent /*= NULL*/) 
    : QObject(parent), d(new HttpServerPrivate)
{
    d->threadedServer = new Tufao::HttpServer();
}

HttpServerStarter::~HttpServerStarter()
{
    delete d->threadedServer;
    delete d;
    d = 0;
}

void HttpServerStarter::start()
{
    DicomNodeConfig cfg;
    const int port = cfg.httpPort();
    qDebug() << "HTTP server listening on "<< port;
    d->threadedServer->run(32, port, createHandler);
}

void HttpServerStarter::stop()
{
    d->threadedServer->shutdown();
    qDebug() << "HTTP Server stopped";
}



}
