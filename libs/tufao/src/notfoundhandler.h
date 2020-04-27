#ifndef TUFAO_NOTFOUNDHANDLER_H
#define TUFAO_NOTFOUNDHANDLER_H

#include "abstracthttpserverrequesthandler.h"
#include "httpserverresponse.h"

namespace Tufao {

/*!
  A handler that responds with "Not found" to every request.

  Its purpose is to avoid boilerplate code.

  \since
  1.0
  */
class NotFoundHandler: public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a NotFoundHandler object.

      \p parent is passed to the QObject constructor.
      */
    explicit NotFoundHandler(QObject *parent = 0);

public slots:
    /*!
      It responds to the request with a not found message.
      */
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList &arg = QStringList());

private:
    static const QByteArray body;
};


} // namespace Tufao

#endif // TUFAO_NOTFOUNDHANDLER_H
