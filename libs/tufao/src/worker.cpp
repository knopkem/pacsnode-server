#include "worker.h"

#include <QtNetwork/QTcpSocket>

#include "httpserverrequest.h"
#include "headers.h"

namespace Tufao {

Worker::Worker()
{
    connect(this, &Worker::newConnection, this, &Worker::onNewConnection, Qt::QueuedConnection);
}

void Worker::setFactory(HttpServer::Factory factory)
{
    factoryMutex.lock();
    this->factory = factory;
    factoryMutex.unlock();
}

void Worker::addConnection(qintptr socketDescriptor)
{
    emit newConnection(socketDescriptor);
}

void Worker::onNewConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    HttpServerRequest *handle = new HttpServerRequest(socket, this);

    connect(handle, SIGNAL(ready()), this, SLOT(onRequestReady()));
    connect(socket, SIGNAL(disconnected()), handle, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Worker::onRequestReady()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());

    QAbstractSocket* socket = request->socket();
    HttpServerResponse *response = new HttpServerResponse(socket, request->responseOptions(), this);

    connect(socket, SIGNAL(disconnected()), response, SLOT(deleteLater()));
    connect(response, SIGNAL(finished()), response, SLOT(deleteLater()));

    if (request->headers().contains("Expect", "100-continue")) {
        response->writeContinue();
    }

    AbstractHttpServerRequestHandler* h = factory();
    h->handleRequest(request, response);
    connect(response, SIGNAL(finished()), h, SLOT(deleteLater()));
}

}
