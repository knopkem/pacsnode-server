#include "mainServer.h"

#include <QtCore>
#include <QCoreApplication>

#include "dicomServer.h"
#include "workDispatcher.h"
#include "httpServerStarter.h"

namespace pacsnode {

class MainServerPrivate 
{
public:
    DicomServer* dicomServer;
    WorkDispatcher* dispatcher;
    HttpServerStarter* httpServer; 
};

MainServer::MainServer( QObject* parent /*= NULL*/ ) : QObject(parent), d(new MainServerPrivate)
{
    // create and start DICOM server
    d->dicomServer = new DicomServer();

    // create image processor
    d->dispatcher = new WorkDispatcher();

    // create and start HTTP server
    d->httpServer = new HttpServerStarter();
}

MainServer::~MainServer()
{
    delete d->httpServer;
    delete d->dispatcher;
    delete d->dicomServer;
    delete d;
    d = 0;
}

void MainServer::start()
{
    d->dicomServer->start();
    d->dispatcher->start();
    d->httpServer->start();
}

void MainServer::stop()
{
    d->httpServer->stop();
    d->dispatcher->stop();
    d->dicomServer->stop();

    // now quit the application
    qApp->quit();
}

}
