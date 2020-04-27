/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

#ifndef httpserver_h__
#define httpserver_h__


#include <functional>

#include <QTcpServer>
#include <QVector>

#include "abstracthttpserverrequesthandler.h"

namespace Tufao {
class HttpServerRequest;
class HttpServerResponse;
class Worker;

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    typedef std::function<Tufao::AbstractHttpServerRequestHandler*()> Factory;

    explicit HttpServer(QObject *parent = 0);

    void run(int threadsNumber, int port, Factory factory);

    void shutdown();

signals:
    void newConnection(int socketDescriptor);

protected:
    void incomingConnection(qintptr handle);

private:
    QVector<Worker*> workers;
    QVector<QThread*> threads;

    int i;
};

} // namespace Tufao

#endif // httpserver_h__

