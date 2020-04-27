#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMutex>

#include "httpserver.h"

namespace Tufao {

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker();

    void setFactory(HttpServer::Factory factory);
    void addConnection(qintptr socketDescriptor);

signals:
    void newConnection(qintptr socketDescriptor);

private slots:
    void onNewConnection(qintptr socketDescriptor);
    void onRequestReady();

private:
    HttpServer::Factory factory;
    QMutex factoryMutex;
};

}
#endif // WORKER_H
